#pragma once

#pragma warning(push)
#pragma warning(disable: 4200)
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#pragma warning(pop)

#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>

namespace WinAPI = SKSE::WinAPI;
namespace logger = SKSE::log;

using namespace std::literals;
// using float instead of double
using json = nlohmann::basic_json<nlohmann::ordered_map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;

#define JSX_ALL(Type, ...)                                                                                                                                   \
	friend void from_json(const json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) } \
	friend void to_json(json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) }

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
#include "STL.h"
