#pragma once

#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"

#include <type_traits>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifndef NDEBUG
#include <spdlog/sinks/base_sink.h>
#else
#include <spdlog/sinks/basic_file_sink.h>
#endif

using namespace std::literals;

namespace logger
{
	using namespace SKSE::log;
}

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
#include "Settings.h"