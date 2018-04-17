#include <string>

#include "envoy/registry/registry.h"

#include "common/config/solo_well_known_names.h"

#include "extensions/filters/http/common/empty_http_filter_config.h"

#include "functional_base.pb.h"

namespace Envoy {
namespace Server {
namespace Configuration {

class FunctionBaseFilterFactory
    : public Extensions::HttpFilters::Common::EmptyHttpFilterConfig {
public:
  // Server::Configuration::NamedHttpFilterConfigFactory
  std::string name() override {
    return Config::SoloCommonFilterNames::get().FUNCTIONAL_ROUTER;
  }

  // Server::Configuration::EmptyHttpFilterConfig
  HttpFilterFactoryCb createFilter(const std::string &,
                                   FactoryContext &) override {
    return [](Http::FilterChainFactoryCallbacks &) -> void {};
  }

  virtual ProtobufTypes::MessagePtr createEmptyRouteConfigProto() {
    return ProtobufTypes::MessagePtr{
        new envoy::api::v2::filter::http::FunctionalFilterRouteConfig()};
  }
};

/**
 * Static registration for the Google Cloud Functions filter. @see
 * RegisterFactory.
 */
static Registry::RegisterFactory<FunctionBaseFilterFactory,
                                 NamedHttpFilterConfigFactory>
    register_;

} // namespace Configuration
} // namespace Server
} // namespace Envoy
