#pragma once

namespace Script
{
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
	using ArrayPtr = RE::BSTSmartPointer<RE::BSScript::Array>;
	using CallbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;
	using Args = RE::BSScript::IFunctionArguments;

	inline ObjectPtr GetObject(RE::TESForm* a_form, const char* a_class)
	{
		auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		auto policy = vm->GetObjectHandlePolicy();
		auto handle = policy->GetHandleForObject(a_form->GetFormType(), a_form);
		ObjectPtr object = nullptr;
		bool found = vm->FindBoundObject(handle, a_class, object);
		return found ? object : nullptr;
	}

	/*
	template <class T>
	inline T GetVariable(ObjectPtr a_obj, RE::BSFixedString a_var)
	{
		auto var = a_obj->GetVariable(a_var);
		return RE::BSScript::UnpackValue<T>(var);
	}

	template <class T>
	inline void SetVariable(ObjectPtr a_obj, RE::BSFixedString a_prop, T a_val)
	{
		auto var = a_obj->GetVariable(a_prop);
		if (!var)
			return;

		RE::BSScript::PackValue(var, a_val);
	}*/

	template <class T>
	inline T GetProperty(ObjectPtr a_obj, RE::BSFixedString a_prop)
	{
		auto var = a_obj->GetProperty(a_prop);
		return RE::BSScript::UnpackValue<T>(var);
	}

	template <class T>
	inline void SetProperty(ObjectPtr a_obj, RE::BSFixedString a_prop, T a_val)
	{
		auto var = a_obj->GetProperty(a_prop);
		if (!var)
			return;

		RE::BSScript::PackValue(var, a_val);
	}

	/*
	inline void CallMethod(ObjectPtr a_obj, std::string a_class, std::string a_func, Args* a_args)
	{
		auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> result;
		//vm->DispatchMethodCall(a_obj, a_class, a_func, a_args, result);
	}*/
}
