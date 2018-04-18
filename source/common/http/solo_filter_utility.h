#pragma once

#include "envoy/http/filter.h"
#include "envoy/upstream/cluster_manager.h"

namespace Envoy {
namespace Http {

/**
 * General utilities for HTTP filters.
 *
 * TODO(talnordan): Merge this class into
 * envoy/source/common.http/filter_utility.h.
 */
class SoloFilterUtility {
public:
  // TODO(talnordan): The envoyproxy/envoy convention seems to be not to
  // explicitly delete constructors.
  SoloFilterUtility() = delete;
  SoloFilterUtility(const SoloFilterUtility &) = delete;

  /**
   * Resolve the route entry.
   * @param decoder_callbacks supplies the decoder callback of filter.
   * @return the route entry selected for this request. Note: this will be
   * nullptr if no route was selected.
   */
  static const Router::RouteEntry *
  resolveRouteEntry(StreamDecoderFilterCallbacks *decoder_callbacks);

  /**
   * Resolve the cluster name.
   * @param decoder_callbacks supplies the decoder callback of filter.
   * @return the name of the cluster selected for this request. Note: this will
   * be nullptr if no route was selected.
   */
  static const std::string *
  resolveClusterName(StreamDecoderFilterCallbacks *decoder_callbacks);
};

class PerFilterConfigUtilBase {
protected:
  PerFilterConfigUtilBase(const std::string &filter_name)
      : filter_name_(filter_name) {}
  const Protobuf::Message *
  getPerFilterBaseConfig(StreamDecoderFilterCallbacks &decoder_callbacks);

private:
  const std::string &filter_name_;
  Router::RouteConstSharedPtr route_info_{};
};

template <class ConfigProto>
class PerFilterConfigUtil : PerFilterConfigUtilBase {

  static_assert(std::is_base_of<Protobuf::Message, ConfigProto>::value,
                "ConfigProto must be a subclass of Protobuf::Message");

public:
  PerFilterConfigUtil(const std::string &filter_name)
      : PerFilterConfigUtilBase(filter_name) {}

  const ConfigProto *
  getPerFilterConfig(StreamDecoderFilterCallbacks &decoder_callbacks) {
    return dynamic_cast<const ConfigProto *>(
        getPerFilterBaseConfig(decoder_callbacks));
  }
};

} // namespace Http
} // namespace Envoy
