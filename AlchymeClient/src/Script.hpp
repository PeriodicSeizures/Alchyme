#ifndef SCRIPT_H
#define SCRIPT_H

#include <sol/sol.hpp>

namespace ScriptManager {
	void Init();
	lua_State* GetLuaState();
	void UnInit();

	namespace Event {
		/// Event calls
		void OnHandshake();
	}
};

#endif
