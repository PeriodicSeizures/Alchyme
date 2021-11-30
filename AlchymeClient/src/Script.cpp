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
	const std::function<void(float)> onUpdate;
	const std::function<void()> onHandshake;
};

std::vector<Script> scripts;
sol::state lua;

namespace ScriptManager {

	namespace Api {

		void RegisterScript(sol::table scriptTable) {
			Script script = {
				scriptTable["onEnable"].get_or(std::function<void()>()),
				scriptTable["onDisable"].get_or(std::function<void()>()),
				scriptTable["onUpdate"].get_or(std::function<void(float)>()),
				scriptTable["onHandshake"].get_or(std::function<void()>()),
			};
			scripts.push_back(script);
			script.onEnable();
		}

		void ConnectToServer(std::string address) {
			std::cout << "Attempt to connect to server: " << address << "\n";

			auto at = address.find(':');
			std::string port = "8001";
			if (at != std::string::npos) {
				port = address.substr(at + 1);
				address = address.substr(0, at);
			}

			GetClient()->Connect(address, port);
		}

		void ForwardPeerInfo(std::string username, std::string password) {
			GetClient()->ForwardPeerInfo(username, password);
		}

	}

	void Init() {
		std::string scriptCode;
		if (Rml::GetFileInterface()->LoadFile("scripts/entry.lua", scriptCode)) {

			// State
			scripts.clear();
			lua = sol::state();
			lua.open_libraries();

			Rml::Lua::Initialise(lua.lua_state());

			auto apiTable = lua["Alchyme"].get_or_create<sol::table>(); // lua.create_table_with(420);

			apiTable["RegisterScript"] = ScriptManager::Api::RegisterScript;
			apiTable["ConnectToServer"] = ScriptManager::Api::ConnectToServer;
			apiTable["ForwardPeerInfo"] = ScriptManager::Api::ForwardPeerInfo;



			// Test global user states to load stuff from script
			// ...

			lua.safe_script(scriptCode);
		}
	}

	lua_State* ScriptManager::GetLuaState() {
		return lua.lua_state();
	}

	void ScriptManager::UnInit() {
		scripts.clear();
	}

	namespace Event {
		/// Event forward calls
		void OnHandshake() {
			for (auto& script : scripts) {
				script.onHandshake();
			}
		}

		void OnUpdate(float delta) {
			for (auto& script : scripts) {
				script.onUpdate(delta);
			}
		}
	}

}