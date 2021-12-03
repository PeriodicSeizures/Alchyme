#ifndef CLIENT_H
#define CLIENT_H

#include "IClient.h"
#include "World.h"
#include "MyRenderInterface.h"
#include "MySystemInterface.h"
#include "MyFileInterface.h"

enum class GameState {
	MainMenu,
	Playing
};

class Client : public IClient {

	/// Client initialization stuff
	SDL_Window* m_sdlWindow;
	SDL_GLContext* m_sdlGLContext;
	SDL_Renderer* m_sdlRenderer;
	


	Rml::Context* m_rmlContext;
	std::unique_ptr<MyRenderInterface> m_renderInterface;
	std::unique_ptr<MySystemInterface> m_systemInterface;
	std::unique_ptr<MyFileInterface> m_fileInterface;

	

	/// Client connection stuff
	//std::thread m_thrPassword;
	bool serverAwaitingPeerInfo;
	const std::string m_version = "1.0.0";
	NetPeer m_peer;



	/// Initializer methods
	void InitSDL();
	void InitGLEW();
	void InitRML();


	//void PasswordCallback(Rpc* rpc);

	void RPC_ClientHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc, size_t peerUid, size_t worldSeed, size_t worldTime);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_Error(Rpc* rpc, std::string s);

	/*
	* 
	* server implemtation
	* 
	*/
	void Update(float delta) override;
	void ConnectCallback(Rpc* rpc, ConnResult res) override;
	void DisconnectCallback(Rpc* rpc) override;

public:
	void ForwardPeerInfo(std::string username, std::string password);

	Client();
	~Client();

	static Client* GetClient();
	static void RunClient();
};

#endif
