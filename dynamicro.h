#ifndef _DYNAMICRO_H_
#define _DYNAMICRO_H_

#include <string>
#include <map>
#include <functional>
#include <type_traits>
#include <utility>


#if defined(__unix__)
#include <dlfcn.h>
#include <iostream>
#elif defined(_WIN32)
#include <Windows.h>
#endif

class Dynamicro {
public:

	Dynamicro() : m_hMod(nullptr) {
	}

	~Dynamicro() {
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


	template<typename T>
	std::function<T> get(const std::string &funcName) {
		auto it = m_map.find(funcName);
		if (it == m_map.end()) {

#ifdef _WIN32
			GetProcAddress(m_hMod, funcName.c_str());
#elif __unix__
			dlsym(m_hMod, funcName.data());
#endif // WIN32

			if (!m_hMod)
				return nullptr;
			m_map[funcName] = m_hMod;
			it = m_map.find(funcName);
		}

		return std::function<T>((T *)(it->second));
	};

	template<typename T, typename... Args>
	typename std::result_of<std::function<T>(Args...)>::type exec(const std::string &funcName, Args &&... args) {
		auto f = get<T>(funcName);
		if (f == nullptr) {
			std::string s = "can not find this function :" + funcName;
			std::cout << s << std::endl;
			throw std::exception();
		}

		return f(std::forward<Args>(args)...);
	};

private:

#ifdef _WIN32
	using moduleType = HMODULE;
	using addressType = FARPROC;
#elif __unix__
	using moduleType = void *;
#endif // WIN32

	moduleType m_hMod;
	std::map<std::string, moduleType> m_map;
};

#endif // !_DYNAMICRO_H_

