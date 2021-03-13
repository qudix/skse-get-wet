#pragma once

namespace Papyrus::PapyrusSettings
{
	using VM = RE::BSScript::IVirtualMachine;

	inline void GetProperties(RE::StaticFunctionTag*)
	{
		auto obj = Prop::GetObject();
		if (!obj)
			return;

		auto set = ::Settings::GetSingleton();
		auto& data = set->data;

		Prop::SetBool(obj, "ApplyGlobal", data.Apply.Global);
		Prop::SetBool(obj, "ApplyPlayer", data.Apply.Player);
		Prop::SetBool(obj, "ApplyNPC", data.Apply.NPC);
		Prop::SetBool(obj, "ApplyFemale", data.Apply.Female);
		Prop::SetBool(obj, "ApplyMale", data.Apply.Male);
		Prop::SetBool(obj, "ApplyBeast", data.Apply.Beast);

		Prop::SetFloat(obj, "ActivityStamina", data.Activity.Stamina);
		Prop::SetFloat(obj, "ActivityMagicka", data.Activity.Magicka);
		Prop::SetFloat(obj, "ActivitySprinting", data.Activity.Sprinting);
		Prop::SetFloat(obj, "ActivityRunning", data.Activity.Running);
		Prop::SetFloat(obj, "ActivitySneaking", data.Activity.Sneaking);
		Prop::SetFloat(obj, "ActivityGalloping", data.Activity.Galloping);
		Prop::SetFloat(obj, "ActivityWorking", data.Activity.Working);

		Prop::SetFloat(obj, "WetnessMin", data.Wetness.Min);
		Prop::SetFloat(obj, "WetnessStart", data.Wetness.Start);
		Prop::SetFloat(obj, "WetnessSoaked", data.Wetness.Soaked);
		Prop::SetFloat(obj, "WetnessMax", data.Wetness.Max);
		Prop::SetFloat(obj, "WetnessRateMax", data.Wetness.RateMax);

		Prop::SetFloat(obj, "GlossinessMin", data.Glossiness.Min);
		Prop::SetFloat(obj, "GlossinessMax", data.Glossiness.Max);
		Prop::SetBool(obj, "GlossinessHead", data.Glossiness.Head);
		Prop::SetBool(obj, "GlossinessBody", data.Glossiness.Body);
		Prop::SetBool(obj, "GlossinessHands", data.Glossiness.Hands);
		Prop::SetBool(obj, "GlossinessFeet", data.Glossiness.Feet);
		Prop::SetBool(obj, "GlossinessOther", data.Glossiness.Other);

		Prop::SetFloat(obj, "SpecularMin", data.Specular.Min);
		Prop::SetFloat(obj, "SpecularMax", data.Specular.Max);
		Prop::SetBool(obj, "SpecularHead", data.Specular.Head);
		Prop::SetBool(obj, "SpecularBody", data.Specular.Body);
		Prop::SetBool(obj, "SpecularHands", data.Specular.Hands);
		Prop::SetBool(obj, "SpecularFeet", data.Specular.Feet);
		Prop::SetBool(obj, "SpecularOther", data.Specular.Other);
	}

	inline void SetProperties(RE::StaticFunctionTag*)
	{
		auto obj = Prop::GetObject();
		if (!obj)
			return;

		auto set = ::Settings::GetSingleton();
		auto& data = set->data;

		data.Apply.Global = Prop::GetBool(obj, "ApplyGlobal");
		data.Apply.Player = Prop::GetBool(obj, "ApplyPlayer");
		data.Apply.NPC = Prop::GetBool(obj, "ApplyNPC");
		data.Apply.Female = Prop::GetBool(obj, "ApplyFemale");
		data.Apply.Male = Prop::GetBool(obj, "ApplyMale");
		data.Apply.Beast = Prop::GetBool(obj, "ApplyBeast");

		data.Activity.Stamina  = Prop::GetFloat(obj, "ActivityStamina");
		data.Activity.Magicka = Prop::GetFloat(obj, "ActivityMagicka");
		data.Activity.Sprinting = Prop::GetFloat(obj, "ActivitySprinting");
		data.Activity.Running = Prop::GetFloat(obj, "ActivityRunning");
		data.Activity.Sneaking = Prop::GetFloat(obj, "ActivitySneaking");
		data.Activity.Galloping = Prop::GetFloat(obj, "ActivityGalloping");
		data.Activity.Working = Prop::GetFloat(obj, "ActivityWorking");

		data.Wetness.Min = Prop::GetFloat(obj, "WetnessMin");
		data.Wetness.Start = Prop::GetFloat(obj, "WetnessStart");
		data.Wetness.Soaked = Prop::GetFloat(obj, "WetnessSoaked");
		data.Wetness.Max = Prop::GetFloat(obj, "WetnessMax");
		data.Wetness.RateMax = Prop::GetFloat(obj, "WetnessRateMax");

		data.Glossiness.Max = Prop::GetFloat(obj, "GlossinessMin");
		data.Glossiness.Min = Prop::GetFloat(obj, "GlossinessMax");
		data.Glossiness.Head = Prop::GetBool(obj, "GlossinessHead");
		data.Glossiness.Body = Prop::GetBool(obj, "GlossinessBody");
		data.Glossiness.Hands = Prop::GetBool(obj, "GlossinessHead");
		data.Glossiness.Feet = Prop::GetBool(obj, "GlossinessFeet");
		data.Glossiness.Other = Prop::GetBool(obj, "GlossinessOther");

		data.Specular.Max = Prop::GetFloat(obj, "SpecularMin");
		data.Specular.Min = Prop::GetFloat(obj, "SpecularMax");
		data.Specular.Head = Prop::GetBool(obj, "SpecularHead");
		data.Specular.Body = Prop::GetBool(obj, "SpecularBody");
		data.Specular.Hands = Prop::GetBool(obj, "SpecularHands");
		data.Specular.Feet = Prop::GetBool(obj, "SpecularFeet");
		data.Specular.Other = Prop::GetBool(obj, "SpecularOther");
	}

	inline void Bind(VM& a_vm)
	{
		const auto obj = "qdx_gw"sv;

		BIND(GetProperties);
		BIND(SetProperties);
	}
}
