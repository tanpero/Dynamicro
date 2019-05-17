#ifndef _TFC_DYNAMICRO_H_
#define _TFC_DYNAMICRO_H_

/*
* Copyright (c) Tanpero 2019
* All rights reserved.
*
* Provide a set of useful methods to easily call
* the interface in the dynamic link library.
*
* A subproject of Tanpero Foundation Classes (TFC).
* Under GPL-v3 license.
*/

#include <string>
#include <map>
#include <functional>

#ifdef _WIN32
#	include <Windows.h>
#elif defined(UNIX)
#	include <dlfcn.h>
#endif


class Dynamicro
{
public:

	Dynamicro() : m_hMod(nullptr)
	{
	}

	~Dynamicro()
	{
		unload();
	}

	inline bool load(const std::string& dllPath)
	{
#ifdef WIN32
		m_hMod = LoadLibraryA(dllPath.data());
#elif defined (__GNUC__) && defined(__unix__)
		m_hMod = dlopen(dllPath.data(), RTLD_NOW);
		dlerror();
#endif // WIN32

		if (m_hMod == nullptr)
		{
			printf("load library failed\n");
			return false;
		}

		return true;
	}

	inline bool unload()
	{
		if (m_hMod == nullptr)
			return true;

#ifdef WIN32
		auto b = FreeLibrary(m_hMod);
#elif defined (__GNUC__) && defined(__unix__)
		auto b = dlclose(m_hMod);
#endif // WIN32
		
		if (!b)
			return false;

		m_hMod = nullptr;
		return true;
	}

	template <typename T>
	inline std::function<T> get(const std::string& funcName)
	{
		std::map<std::string, moduleType>::iterator it = m_map.find(funcName);
		if (it == m_map.end())
		{

#ifdef WIN32
			auto addr = GetProcAddress(m_hMod, funcName.c_str());
#elif defined (__GNUC__) && defined(__unix__)
			auto addr = dlsym(m_hMod, funcName.data());
#endif // WIN32

			if (!addr)
				return nullptr;
			m_map.insert(std::make_pair(funcName, addr));
			it = m_map.find(funcName);
		}

		return std::function<T>((T*)(it->second));
	}

	template <typename T, typename... Args>
	inline typename std::result_of<std::function<T>(Args...)>::type
		exec(const std::string& funcName, Args&&... args)
	{
		auto f = get<T>(funcName);
		if (f == nullptr)
		{
			std::string s = "can not find this function " + funcName;
			throw std::exception(s.c_str());
		}

		return f(std::forward<Args>(args)...);
	}

private:

#ifdef WIN32
	using moduleType = HMODULE;
#elif defined (__GNUC__) && defined(__unix__)
	using moduleType = void*;
#endif // WIN32

	moduleType m_hMod;
	std::map<std::string, moduleType> m_map;
};

#endif // !_TFC_DYNAMICRO_H_

