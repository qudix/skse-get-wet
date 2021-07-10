#include "Game/Config.h"
#include "Game/Meta.h"
#include "Game/Presets.h"
#include "Papyrus/Papyrus.h"
#include "Serialization/Manager.h"
#include "Serialization/Event.h"

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		// Config
		auto& config = Config::GetSingleton();
		if (!config.Load()) {
			logger::info("Config did not load correctly");
			return;
		}

		// Presets
		auto& presets = Presets::GetSingleton();
		if (!presets.Load()) {
			logger::info("Presets did not load correctly");
			return;
		}

		// Meta
		auto& meta = Meta::GetSingleton();
		meta.Setup();

		// Event
		Event::Register();
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	auto path = logger::log_directory();
	if (!path)
		return false;

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);

	logger::info("{} v{}"sv, Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical("Unsupported runtime version {}"sv, ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	Papyrus::Bind();

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", OnInit);

	const auto serial = SKSE::GetSerializationInterface();
	serial->SetUniqueID(Serialization::kGetWet);
	serial->SetSaveCallback(Serialization::Save);
	serial->SetLoadCallback(Serialization::Load);
	serial->SetRevertCallback(Serialization::Revert);

	logger::info("{} loaded"sv, Version::PROJECT);

	return true;
}
