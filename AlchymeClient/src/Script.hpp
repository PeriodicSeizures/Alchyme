#ifndef SCRIPT_H
#define SCRIPT_H

#include <sol/sol.hpp>

namespace ScriptInterface {
	void Init();

	lua_State* GetLuaState();

	void UnInit();

};

#endif
