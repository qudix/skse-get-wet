#pragma once

#include "Util/Util.h"

namespace SettingsActor
{
	json GetActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::FormToString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		for (auto& obj : actors) {
			auto actor = obj.value("Actor", "");
			if (!actor.empty() && actor == actor_id) {
				return obj;
			}
		}

		return {};
	}

	void SetActorSetting(RE::Actor* a_actor, std::string a_key, float a_value)
	{
		auto actor_id = Util::FormToString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		for (auto& [index, obj] : actors.items()) {
			auto actor = obj.value("Actor", "");
			if (!actor.empty() && actor == actor_id) {
				obj[a_key] = a_value;
				settings->Save();
				break;
			}
		}
	}

	void AddActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::FormToString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");
		auto obj = json::object();

		obj["Actor"] = actor_id;
		obj["VisualGlossinessMin"] = settings->Get<float>("VisualGlossinessMin");
		obj["VisualGlossinessMax"] = settings->Get<float>("VisualGlossinessMax");
		obj["VisualSpecularMin"] = settings->Get<float>("VisualSpecularMin");
		obj["VisualSpecularMax"] = settings->Get<float>("VisualSpecularMax");

		actors.push_back(obj);
		settings->Save();
	}

	void RemoveActorSettings(RE::Actor* a_actor)
	{
		auto actor_id = Util::FormToString(a_actor);
		auto settings = Settings::GetSingleton();
		auto& actors = settings->Get("Actors");

		int index = -1;
		for (auto& it : actors.items()) {
			auto actor = it.value().at("Actor").get<std::string>();
			if (!actor.empty() && actor == actor_id) {
				index = std::stoi(it.key());
				break;
			}
		}

		if (index != -1) {
			actors.erase(index);
			settings->Save();
		}
	}
}