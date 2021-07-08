#include "Serialization/Manager.h"

#include "Game/Meta.h"

namespace Serialization
{
	std::string DecodeType(uint32_t a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(uint32_t);

		std::string sig;
		sig.resize(SIZE);
		char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}

		return sig;
	}

	void Save(SKSE::SerializationInterface* a_intfc)
	{
		auto& meta = Meta::GetSingleton();
		if (!meta.Save(a_intfc, kMeta, kSerializationVersion)) {
			logger::info("Failed to save Meta regs!"sv);
		}

		logger::info("Finished saving data"sv);
	}

	void Load(SKSE::SerializationInterface* a_intfc)
	{
		uint32_t type;
		uint32_t version;
		uint32_t length;

		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != kSerializationVersion) {
				logger::critical("Loaded data is outdated! Read ({}), expected ({}) for type code ({})"sv,
					version, kSerializationVersion, DecodeType(type));
				continue;
			}

			switch (type) {
			case kMeta:
				{
					auto& meta = Meta::GetSingleton();
					if (!meta.Load(a_intfc, length)) {
						logger::info("Failed to load Meta regs!"sv);
					}
				}
				break;
			default:
				logger::critical("Unrecognized record type ({})!"sv, DecodeType(type));
				break;
			}
		}

		logger::info("Finished loading data"sv);
	}

	void Revert(SKSE::SerializationInterface* a_intfc)
	{
		auto& meta = Meta::GetSingleton();
		meta.Revert(a_intfc);
	}
}
