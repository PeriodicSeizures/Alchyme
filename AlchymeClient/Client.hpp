#pragma once
#include "IClient.h"

namespace {
	void RPC_Print(Rpc* rpc, int a) {
		std::cout << "Remote arguments: " << a << "\n";
	}

	void RPC_NoArgs(Rpc* rpc) {
		std::cout << "I was remotely called with no args!\n";
	}
}

class Client : public IClient {
	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		rpc->Register("print", new Method(RPC_Print));
		rpc->Register("noargs", new Method(RPC_NoArgs));
	}

	void DisconnectCallback(Rpc* rpc) override {}
};
