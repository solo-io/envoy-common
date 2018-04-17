licenses(["notice"])  # Apache 2

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "route_fault_lib",
    srcs = ["route_fault.cc"],
    hdrs = ["route_fault.h"],
    repository = "@envoy",
    deps = [
        "//source/common/config:solo_well_known_names_lib",
        "//source/common/http/filter:route_fault_filter_lib",
        "@envoy//include/envoy/registry",
        "@envoy//include/envoy/server:filter_config_interface",
        "@envoy//source/common/config:filter_json_lib",
        "@envoy//source/common/http/filter:fault_filter_lib",
    ],
)

envoy_cc_library(
    name = "functional_stream_decoder_lib",
    srcs = ["functional_stream_decoder.cc"],
    repository = "@envoy",
    deps = [
        "//source/common/config:solo_well_known_names_lib",
        "//source/common/http:functional_stream_decoder_base_lib",
        "@envoy//include/envoy/registry",
        "@envoy//include/envoy/server:filter_config_interface",
        "@envoy//source/common/config:filter_json_lib",
        "@envoy//source/extensions/filters/http/common:empty_http_filter_config_lib",
    ],
)
