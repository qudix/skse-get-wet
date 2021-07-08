#pragma once

class Config final
{
public:
	struct Data
	{
		float ActStamina;
		float ActMagicka;
		float ActSprinting;
		float ActRunning;
		float ActSneaking;
		float ActGalloping;
		float ActWorking;

		float WetnessMin;
		float WetnessMax;
		float WetnessRateMax;

		float GlossinessMin;
		float GlossinessMax;
		bool GlossinessHead;
		bool GlossinessBody;
		bool GlossinessHands;
		bool GlossinessFeet;
		bool GlossinessOther;

		float SpecularMin;
		float SpecularMax;
		bool SpecularHead;
		bool SpecularBody;
		bool SpecularHands;
		bool SpecularFeet;
		bool SpecularOther;

		bool ApplyGlobal;
		bool ApplyPlayer;
		bool ApplyNPC;
		bool ApplyMale;
		bool ApplyFemale;
		bool ApplyBeast;

		bool MiscDebug;

		JSX_ALL(Data, ActStamina, ActMagicka, ActSprinting, ActRunning, ActSneaking, ActGalloping, ActWorking,
			WetnessMin, WetnessMax, WetnessRateMax, GlossinessMin, GlossinessMax, GlossinessHead, GlossinessBody,
			GlossinessHands, GlossinessFeet, GlossinessOther, SpecularMin, SpecularMax, SpecularHead, SpecularBody,
			SpecularHands, SpecularFeet, SpecularOther, ApplyGlobal, ApplyPlayer, ApplyNPC, ApplyMale, ApplyFemale,
			ApplyBeast, MiscDebug)
	};

	[[nodiscard]]
	static Config& GetSingleton() noexcept;

	bool Load();
	void Save();

public:
	Data defu;
	Data data;

private:
	Config() = default;
	Config(const Config&) = delete;
	Config(Config&&) = delete;

	inline ~Config() { Save(); };

	Config& operator=(const Config&) = delete;
	Config& operator=(Config&&) = delete;
};
