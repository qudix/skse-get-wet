#pragma once

namespace Serial
{
	inline void WriteString(SKSE::SerializationInterface* a_intfc, std::string const& a_string)
	{
		uint32_t length = static_cast<uint32_t>(a_string.length());
		a_intfc->WriteRecordData(&length, sizeof(length));
		a_intfc->WriteRecordData(a_string.c_str(), length);
	}

	template <typename T>
	inline void WriteContainer(SKSE::SerializationInterface* a_intfc, T const& a_container)
	{
		uint32_t size = static_cast<uint32_t>(a_container.size());
		a_intfc->WriteRecordData(&size, sizeof(size));
		for (auto const& kvp : a_container)
			a_intfc->WriteRecordData(&kvp, sizeof(kvp));
	}

	template <typename T>
	inline T Read(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
	{
		uint32_t size = sizeof(T);
		if (size > a_length)
			throw std::length_error("Savegame data ended unexpected");
		a_length -= size;
		T result{};
		a_intfc->ReadRecordData(&result, size);
		return result;
	}

	inline std::string ReadString(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
	{
		uint32_t strlength = Read<uint32_t>(a_intfc, a_length);
		if (strlength > a_length)
			throw std::length_error("Savegame data ended unexpected");
		a_length -= strlength;
		char* buf = new char[strlength + 1];
		a_intfc->ReadRecordData(buf, strlength);
		buf[strlength] = '\0';
		std::string result(buf);
		delete[] buf;
		return result;
	}

	template <typename T>
	inline bool Write(SKSE::SerializationInterface* a_intfc, const T* a_data)
	{
		return a_intfc->WriteRecordData(a_data, sizeof(T));
	}

	template <typename T>
	inline bool Read(SKSE::SerializationInterface* a_intfc, T* a_data)
	{
		return a_intfc->ReadRecordData(a_data, sizeof(T)) > 0;
	}
}
