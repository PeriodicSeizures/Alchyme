#include "Script.hpp"
#include <RmlUi/Core.h>
#include <RmlUi/Lua.h>
#include <sol/sol.hpp>
#include <filesystem>
#include "Client.hpp"

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
			LOG(INFO) << "Attempt to connect to server: " << address << "\n";

			auto at = address.find(':');
			std::string port = "8001";
			if (at != std::string::npos) {
				port = address.substr(at + 1);
				address = address.substr(0, at);
			}

			Client::GetClient()->Connect(address, port);
		}

		void DisconnectFromServer() {
			Client::GetClient()->Disconnect();
		}

		void ForwardPeerInfo(std::string username, std::string password) {
			Client::GetClient()->ForwardPeerInfo(username, password);
		}



		//std::string GetHostName()

	}

	void Init() {
		std::string scriptCode;
		if (Rml::GetFileInterface()->LoadFile("scripts/entry.lua", scriptCode)) {

			// State
			lua = sol::state();
			lua.open_libraries();

			Rml::Lua::Initialise(lua.lua_state());

			auto apiTable = lua["Alchyme"].get_or_create<sol::table>();

			apiTable["RegisterScript"] = ScriptManager::Api::RegisterScript;
			apiTable["ConnectToServer"] = ScriptManager::Api::ConnectToServer;
			apiTable["DisconnectFromServer"] = ScriptManager::Api::DisconnectFromServer;
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
				if (script.onHandshake) // check is mandatory to avoid std::bad_function_call
										// if function is empty
					
					script.onHandshake();
			}
		}

		void OnUpdate(float delta) {
			for (auto& script : scripts) {
				if (script.onUpdate)
					script.onUpdate(delta);				
			}
		}
	}

}