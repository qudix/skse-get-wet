#pragma once

namespace Prop
{
	using Object = RE::BSScript::Object;
	Object* GetObject()
	{
		auto quest = Actor::m_QuestConfig;
		if (!quest)
			return nullptr;

		auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		auto policy = vm->GetObjectHandlePolicy();
		auto handle = policy->GetHandleForObject(RE::FormType::Quest, quest);

		RE::BSTSmartPointer<Object> object_ptr = nullptr;
		bool found = vm->FindBoundObject(handle, "qdx_gw_QuestConfig", object_ptr);
		return found ? object_ptr.get() : nullptr;
	}

	bool GetBool(Object* a_obj, RE::BSFixedString a_prop)
	{
		auto var = a_obj->GetProperty(a_prop);
		return var ? var->GetBool() : false;
	}

	float GetFloat(Object* a_obj, RE::BSFixedString a_prop)
	{
		auto var = a_obj->GetProperty(a_prop);
		return var ? var->GetFloat() : 0.0f;
	}

	void SetBool(Object* a_obj, RE::BSFixedString a_prop, bool a_val)
	{
		auto var = a_obj->GetProperty(a_prop);
		if (var)
			var->SetBool(a_val);
	}

	void SetFloat(Object* a_obj, RE::BSFixedString a_prop, float a_val)
	{
		auto var = a_obj->GetProperty(a_prop);
		if (var)
			var->SetFloat(a_val);
	}
}
