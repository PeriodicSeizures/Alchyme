#ifndef ALCHYME_CLIENT_H
#define ALCHYME_CLIENT_H

#include "AlchymeGame.h"
#include "NetPeer.h"
#include "MyRenderInterface.h"
#include "MySystemInterface.h"
#include "MyFileInterface.h"

class AlchymeClient : public AlchymeGame {
	SDL_Window* m_sdlWindow = nullptr;
	SDL_GLContext* m_sdlGLContext = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;

	Rml::Context* m_rmlContext = nullptr;
	std::unique_ptr<MyRenderInterface> m_renderInterface;
	std::unique_ptr<MySystemInterface> m_systemInterface;
	std::unique_ptr<MyFileInterface> m_fileInterface;

	bool serverAwaitingLogin = false; 
	const std::string m_version = "1.0.0";
	std::unique_ptr<NetPeer> m_peer;

	//std::unique_ptr<Rpc> m_rpc;

public:
	static AlchymeClient* Get();

	AlchymeClient();

	void Start() override;
	void Stop() override;

	void Connect(std::string host, std::string port);

	void SendLogin(std::string username, std::string password);

private:
	void Update(float delta) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	void InitSDL();
	void InitGLEW();
	void InitRML();

	void RPC_ClientHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc, size_t peerUid, size_t worldSeed, size_t worldTime);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_Error(Rpc* rpc, std::string s);

};

#endif