#include <string>

#include "envoy/registry/registry.h"

#include "common/config/solo_well_known_names.h"
#include "common/http/functional_stream_decoder_base.h"
#include "common/protobuf/utility.h"

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

  virtual Router::RouteSpecificFilterConfigConstSharedPtr
  createRouteSpecificFilterConfig(const ProtobufWkt::Struct &source) {
    envoy::api::v2::filter::http::FunctionalFilterRouteConfig cfg;
    MessageUtil::jsonConvert(source, cfg);
    auto obj = std::make_shared<Http::FunctionalFilterMixinRouteFilterConfig>();
    obj->function_name_ = cfg.function_name();
    return obj;
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
