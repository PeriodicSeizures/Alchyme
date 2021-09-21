#pragma once
#include "IServer.h"

class Server : public IServer {
	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		rpc->Invoke("print", 69420);
	}

	void DisconnectCallback(Rpc* rpc) override {}

public:
	Server() : IServer(8001) {}
};
