licenses(["notice"])  # Apache 2

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_binary",
    "envoy_cc_library",
    "envoy_cc_test",
    "envoy_package",
)

envoy_package()

load("@envoy_api//bazel:api_build_system.bzl", "api_proto_library")

api_proto_library(
    name = "route_fault_proto",
    srcs = ["route_fault.proto"],
    deps = ["@envoy_api//envoy/config/filter/http/fault/v2:fault"],
)

api_proto_library(
    name = "functional_base_proto",
    srcs = ["functional_base.proto"],
)
