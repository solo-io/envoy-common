#include "common/http/functional_stream_decoder_base.h"

#include "common/config/solo_well_known_names.h"
#include "common/http/filter_utility.h"
#include "common/http/solo_filter_utility.h"

namespace Envoy {
namespace Http {

using Server::Configuration::FactoryContext;

FunctionRetrieverMetadataAccessor::FunctionRetrieverMetadataAccessor(
    Server::Configuration::FactoryContext &ctx, const std::string &childname)
    : cm_(ctx.clusterManager()), childname_(childname),
      per_filter_config_(
          Config::SoloCommonFilterNames::get().FUNCTIONAL_ROUTER) {}

FunctionRetrieverMetadataAccessor::~FunctionRetrieverMetadataAccessor() {}

absl::optional<const std::string *>
FunctionRetrieverMetadataAccessor::getFunctionName() const {
  RELEASE_ASSERT(function_name_);
  return function_name_;
}

absl::optional<const ProtobufWkt::Struct *>
FunctionRetrieverMetadataAccessor::getFunctionSpec() const {
  RELEASE_ASSERT(function_name_);

  const auto &metadata = cluster_info_->metadata();

  const auto filter_it = metadata.filter_metadata().find(
      Config::SoloCommonMetadataFilters::get().FUNCTIONAL_ROUTER);
  if (filter_it == metadata.filter_metadata().end()) {
    return nullptr;
  }
  const auto &filter_metadata_struct = filter_it->second;
  const auto &filter_metadata_fields = filter_metadata_struct.fields();

  const auto functions_it = filter_metadata_fields.find(
      Config::MetadataFunctionalRouterKeys::get().FUNCTIONS);
  if (functions_it == filter_metadata_fields.end()) {
    return nullptr;
  }

  const auto &functionsvalue = functions_it->second;
  if (functionsvalue.kind_case() != ProtobufWkt::Value::kStructValue) {
    return nullptr;
  }

  const auto &functions_struct = functionsvalue.struct_value();
  const auto &functions_struct_fields = functions_struct.fields();

  const auto spec_it = functions_struct_fields.find(*function_name_);
  if (spec_it == functions_struct_fields.end()) {
    return nullptr;
  }

  const auto &specvalue = spec_it->second;
  if (specvalue.kind_case() != ProtobufWkt::Value::kStructValue) {
    return nullptr;
  }

  return &specvalue.struct_value();
}

absl::optional<const ProtobufWkt::Struct *>
FunctionRetrieverMetadataAccessor::getClusterMetadata() const {
  RELEASE_ASSERT(child_spec_);
  return child_spec_;
}

absl::optional<const ProtobufWkt::Struct *>
FunctionRetrieverMetadataAccessor::getRouteMetadata() const {

  if (route_spec_) {
    return route_spec_;
  }

  if (!route_info_) {
    // save the pointer as the metadata is owned by it.
    route_info_ = decoder_callbacks_->route();
    if (!route_info_) {
      return {};
    }
  }

  const Router::RouteEntry *routeEntry = route_info_->routeEntry();
  if (!routeEntry) {
    return {};
  }

  const auto &metadata = routeEntry->metadata();
  const auto filter_it = metadata.filter_metadata().find(childname_);
  if (filter_it != metadata.filter_metadata().end()) {
    route_spec_ = &filter_it->second;
    return route_spec_;
  }

  return {};
}

absl::optional<FunctionRetrieverMetadataAccessor::Result>
FunctionRetrieverMetadataAccessor::tryToGetSpec() {
  route_info_ = decoder_callbacks_->route();
  if (!route_info_) {
    return {};
  }

  const Router::RouteEntry *routeEntry = route_info_->routeEntry();
  if (!routeEntry) {
    return {};
  }

  fetchClusterInfoIfOurs();

  if (!cluster_info_) {
    return {};
  }
  // So now we know this this route is to a functional upstream. i.e. we must be
  // able to do a function route or error. unless passthrough is allowed on the
  // upstream.

  const envoy::api::v2::filter::http::FunctionalFilterRouteConfig
      *filter_config =
          per_filter_config_.getPerFilterConfig(*decoder_callbacks_);
  if (!filter_config) {
    // this cast should never fail, but maybe we don't have a config...
    return canPassthrough()
               ? absl::optional<FunctionRetrieverMetadataAccessor::Result>()
               : Result::Error;
  }

  function_name_ = &filter_config->function_name();

  return Result::Active;
}

bool FunctionRetrieverMetadataAccessor::canPassthrough() {

  const auto &metadata = cluster_info_->metadata();

  const auto filter_it = metadata.filter_metadata().find(
      Config::SoloCommonMetadataFilters::get().FUNCTIONAL_ROUTER);
  if (filter_it == metadata.filter_metadata().end()) {
    return true;
  }
  const auto &filter_metadata_struct = filter_it->second;
  const auto &filter_metadata_fields = filter_metadata_struct.fields();

  const auto passthrough_it = filter_metadata_fields.find(
      Config::MetadataFunctionalRouterKeys::get().PASSTHROUGH);
  if (passthrough_it == filter_metadata_fields.end()) {
    return true;
  }

  const auto &functionsvalue = passthrough_it->second;
  if (functionsvalue.kind_case() != ProtobufWkt::Value::kBoolValue) {
    return true;
  }

  return functionsvalue.bool_value();
}

void FunctionRetrieverMetadataAccessor::fetchClusterInfoIfOurs() {

  if (cluster_info_) {
    return;
  }
  Upstream::ClusterInfoConstSharedPtr cluster_info =
      FilterUtility::resolveClusterInfo(decoder_callbacks_, cm_);

  const auto &metadata = cluster_info->metadata();
  const auto filter_it = metadata.filter_metadata().find(childname_);
  if (filter_it != metadata.filter_metadata().end()) {
    // save the cluster info ptr locally as the specs lives in it.
    cluster_info_ = cluster_info;
    child_spec_ = &filter_it->second;
  }
}

} // namespace Http
} // namespace Envoy
