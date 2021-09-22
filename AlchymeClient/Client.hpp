#pragma once
#include "IClient.h"

namespace {
	void RPC_ClientHandshake(Rpc* rpc, bool needPassword) {

	}

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	//void RPC_Pos(Rpc* rpc, )
}

class Client : public IClient {
	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		//rpc->Register("ClientHandshake", new Method(RPC_ClientHandshake));
		//rpc->Register("Print", new Method(RPC_Print));
	}

	void DisconnectCallback(Rpc* rpc) override {}
};
