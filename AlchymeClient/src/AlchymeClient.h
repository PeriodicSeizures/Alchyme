#ifndef ALCHYME_CLIENT_H
#define ALCHYME_CLIENT_H

#include "AlchymeGame.h"
#include "NetPeer.h"
#include "MyRenderInterface.h"
#include "MySystemInterface.h"
#include "MyFileInterface.h"

class AlchymeClient : public AlchymeGame {
	SDL_Window* m_sdlWindow;
	SDL_GLContext* m_sdlGLContext;
	SDL_Renderer* m_sdlRenderer;

	Rml::Context* m_rmlContext;
	std::unique_ptr<MyRenderInterface> m_renderInterface;
	std::unique_ptr<MySystemInterface> m_systemInterface;
	std::unique_ptr<MyFileInterface> m_fileInterface;

	bool serverAwaitingPeerInfo;
	const std::string m_version = "1.0.0";
	NetPeer m_peer;

	std::unique_ptr<Rpc> m_rpc;

	Rpc* rpc();

public:
	static AlchymeClient* Get();
	static void Run();

	AlchymeClient();

	void Start() override;
	void Stop() override;

	void Connect(std::string host, std::string port);

	void ForwardPeerInfo(std::string username, std::string password);

private:
	void PreUpdate(float delta) override;
	void Update(float delta) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	void RPC_ClientHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc, size_t peerUid, size_t worldSeed, size_t worldTime);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_Error(Rpc* rpc, std::string s);

	void InitSDL();
	void InitGLEW();
	void InitRML();
};

#endif