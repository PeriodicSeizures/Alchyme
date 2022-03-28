#pragma once

#include <sol/sol.hpp>

namespace Alchyme {
	namespace Scripting {
		void Init();
		lua_State* GetLuaState();

		namespace Event {
			/// Event calls
			void OnLogin();
			void OnUpdate(float delta);
		}
	};
}
