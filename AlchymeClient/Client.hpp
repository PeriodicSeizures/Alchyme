#pragma once
#include "IClient.h"

namespace {
	//void RPC_Pos(Rpc* rpc, )
}

class Client : public IClient {

	/*
	* 
	*	RPC DECLARATIONS
	* 
	*/

	void RPC_ClientHandshake(Rpc* rpc, bool needPassword) {
		std::cout << "ClientHandshake()!\n";
		std::cout << "Password needed for server: " << needPassword << "\n";
	}

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	/*
	* 
	* server implemtation
	* 
	*/

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		rpc->Register("ClientHandshake", new Method(this, &Client::RPC_ClientHandshake));
		rpc->Register("Print", new Method(this, &Client::RPC_Print));

		rpc->Invoke("ServerHandshake");
	}

	void DisconnectCallback(Rpc* rpc) override {}
};
