#include "Script.hpp"
#include <RmlUi/Core.h>
#include <RmlUi/Lua.h>
#include <sol/sol.hpp>
#include <filesystem>
#include "Client.hpp"
#include "Game.h"

struct Script {

	const std::function<void()> onEnable;
	const std::function<void()> onDisable;
	//const std::function<void()> onTick;
	// other event functions
};

std::vector<Script> scripts;
sol::state lua;

namespace ScriptApi {

	void Api_RegisterScript(sol::table scriptTable) {
		Script script = {
			scriptTable["onEnable"].get_or(std::function<void()>()),
			scriptTable["onDisable"].get_or(std::function<void()>())
		};
		scripts.push_back(script);
		script.onEnable();
	}

	//void Api_ConnectToServer(std::string host, int port) {
	//
	//	std::cout << "Attempt to connect to server: " << host << ":" << port << "\n";
	//
	//	GetClient()->Connect(host, std::to_string(port));
	//}

	void Api_ConnectToServer(std::string address) {

		std::cout << "Attempt to connect to server: " << address << "\n";

		//auto view = std::string_view(address.c_str()+address);

		auto at = address.find(':');
		std::string port = "8001";
		if (at != std::string::npos) {
			port = address.substr(at + 1);
			address = address.substr(0, at);
		}

		GetClient()->Connect(address, port);
	}

	//void Api_RemoteInvoke(std::string methodName, )

}

void ScriptInterface::Init() {
	std::string scriptCode;
	if (Rml::GetFileInterface()->LoadFile("scripts/entry.lua", scriptCode)) {
		
		//std::cout << "working directory: " <<
			//std::filesystem::current_path() << "\n";
		
		// State
		scripts.clear();
		lua = sol::state();
		lua.open_libraries();

		Rml::Lua::Initialise(lua.lua_state());

		auto apiTable = lua["Alchyme"].get_or_create<sol::table>(); // lua.create_table_with(420);
		
		apiTable["RegisterScript"] = ScriptApi::Api_RegisterScript;
		apiTable["ConnectToServer"] = ScriptApi::Api_ConnectToServer;
		//apiTable["RemoteInvoke"] = ScriptApi::


		
		// Test global user states to load stuff from script
		// ...

		lua.safe_script(scriptCode);
	}
}

lua_State* ScriptInterface::GetLuaState() {
	return lua.lua_state();
}

void ScriptInterface::UnInit() {
	scripts.clear();

	// somehow unload everything
	// to prevent throw
}