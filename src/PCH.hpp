#pragma once

#pragma warning(push)
#pragma warning(disable: 4200)
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#pragma warning(pop)

#include <spdlog/sinks/basic_file_sink.h>

namespace WinAPI = SKSE::WinAPI;
namespace logger = SKSE::log;
namespace fs = std::filesystem;

using namespace std::literals;
using VM = RE::BSScript::Internal::VirtualMachine;
using StackID = RE::VMStackID;
using Severity = RE::BSScript::ErrorLogger::Severity;

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
