#include "common/http/solo_filter_utility.h"

namespace Envoy {
namespace Http {

const Router::RouteEntry *SoloFilterUtility::resolveRouteEntry(
    StreamDecoderFilterCallbacks *decoder_callbacks) {
  Router::RouteConstSharedPtr route = decoder_callbacks->route();
  if (!route) {
    return nullptr;
  }

  return route->routeEntry();
}

const std::string *SoloFilterUtility::resolveClusterName(
    StreamDecoderFilterCallbacks *decoder_callbacks) {
  const Router::RouteEntry *route_entry = resolveRouteEntry(decoder_callbacks);
  if (!route_entry) {
    return nullptr;
  }

  return &route_entry->clusterName();
}

const Protobuf::Message *PerFilterConfigUtilBase::getPerFilterBaseConfig(
    StreamDecoderFilterCallbacks &decoder_callbacks) {
  route_info_ = decoder_callbacks.route();
  if (!route_info_) {
    return {};
  }

  const Protobuf::Message *maybe_filter_config{};

  const Router::RouteEntry *routeEntry = route_info_->routeEntry();
  if (routeEntry) {
    maybe_filter_config = routeEntry->perFilterConfig(filter_name_);
  }

  if (!maybe_filter_config) {
    maybe_filter_config = route_info_->perFilterConfig(filter_name_);
  }

  if (!maybe_filter_config && routeEntry) {
    maybe_filter_config =
        routeEntry->virtualHost().perFilterConfig(filter_name_);
  }
  return maybe_filter_config;
}

} // namespace Http
} // namespace Envoy
