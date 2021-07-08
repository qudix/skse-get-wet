#pragma once

namespace stl
{
	template <class T>
	using optional_ref = std::optional<std::reference_wrapper<T>>;

	inline void write_string(SKSE::SerializationInterface* a_intfc, std::string const& a_string)
	{
		uint32_t length = static_cast<uint32_t>(a_string.length());
		a_intfc->WriteRecordData(&length, sizeof(length));
		a_intfc->WriteRecordData(a_string.c_str(), length);
	}

	template <typename T>
	inline void write_container(SKSE::SerializationInterface* a_intfc, T const& a_container)
	{
		uint32_t size = static_cast<uint32_t>(a_container.size());
		a_intfc->WriteRecordData(&size, sizeof(size));
		for (auto const& kvp : a_container)
			a_intfc->WriteRecordData(&kvp, sizeof(kvp));
	}

	template <typename T>
	inline T read(SKSE::SerializationInterface* a_intfc, uint32_t& a_length)
	{
		uint32_t size = sizeof(T);
		if (size > a_length)
			throw std::length_error("Savegame data ended unexpected");
		a_length -= size;
		T result{};
		a_intfc->ReadRecordData(&result, size);
		return result;
	}

	inline std::string read_string(SKSE::SerializationInterface* a_intfc, uint32_t& length)
	{
		uint32_t strlength = read<uint32_t>(a_intfc, length);
		if (strlength > length)
			throw std::length_error("Savegame data ended unexpected");
		length -= strlength;
		char* buf = new char[strlength + 1];
		a_intfc->ReadRecordData(buf, strlength);
		buf[strlength] = '\0';
		std::string result(buf);
		delete[] buf;
		return result;
	}

	template <typename T>
	inline bool write(SKSE::SerializationInterface* a_intfc, const T* data)
	{
		return a_intfc->WriteRecordData(data, sizeof(T));
	}

	template <typename T>
	inline bool read(SKSE::SerializationInterface* a_intfc, T* a_data)
	{
		return a_intfc->ReadRecordData(a_data, sizeof(T)) > 0;
	}

	inline RE::BSScript::Object* get(RE::TESQuest* a_quest, const char* a_class)
	{
		auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		auto policy = vm->GetObjectHandlePolicy();
		auto handle = policy->GetHandleForObject(RE::FormType::Quest, a_quest);
		RE::BSTSmartPointer<RE::BSScript::Object> object_ptr = nullptr;
		bool found = vm->FindBoundObject(handle, a_class, object_ptr);
		return found ? object_ptr.get() : nullptr;
	}

	template <class T>
	inline T get(RE::BSScript::Object* a_obj, RE::BSFixedString a_prop)
	{
		auto var = a_obj->GetProperty(a_prop);
		return RE::BSScript::UnpackValue<T>(var);
	}

	template <class T>
	inline void set(RE::BSScript::Object* a_obj, RE::BSFixedString a_prop, T a_val)
	{
		auto var = a_obj->GetProperty(a_prop);
		if (!var)
			return;

		RE::BSScript::PackValue(var, a_val);
	}
}
