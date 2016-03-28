package(default_visibility = ["//visibility:public"])

cc_library(
    name = "crc",
    srcs = ["crc.c"],
    hdrs = ["crc.h"],
)

cc_library(
    name = "dxl_hal",
    srcs = ["dxl_hal.c"],
    hdrs = ["dxl_hal.h"],
)

cc_library(
    name = "packet_utils",
    srcs = ["packet_utils.c"],
    hdrs = ["packet_utils.h"],
    deps = [":crc"],
)

cc_library(
    name = "communications",
    srcs = ["communications.c"],
    hdrs = ["communications.h"],
    deps = [":packet_utils", ":dxl_hal"],
)

cc_library(
    name = "high_level_commands",
    srcs = ["high_level_commands.c"],
    hdrs = ["high_level_commands.h", "xl320_control_table.h"],
    deps = [":communications", ":packet_utils"],
)

cc_binary(
    name = "dxl_search",
    srcs = ["dxl_search.c"],
    deps = [":high_level_commands", ":communications", "//external:gflags"],
)
