#pragma once

#include <spdlog/spdlog.h>

#define log_info(fmt, ...) spdlog::info(fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_warn(fmt, ...) spdlog::warn(fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_error(fmt, ...) spdlog::error(fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_debug(fmt, ...) spdlog::debug(fmt __VA_OPT__(, ) __VA_ARGS__)

#define msg_info(fmt, ...)
#define msg_warn(fmt, ...)
#define msg_error(fmt, ...)
#define msg_debug(fmt, ...)