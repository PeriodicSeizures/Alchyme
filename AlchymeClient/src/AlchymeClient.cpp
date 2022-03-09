#include <SDL.h>
#include "Script.hpp"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <optick.h>
#include "AlchymeClient.h"

static std::unique_ptr<AlchymeClient> client;

AlchymeClient *AlchymeClient::Get() {
	return client.get();
}

void AlchymeClient::Run() {
	client = std::make_unique<AlchymeClient>();
	client->Start();
	client.reset();
}

AlchymeClient::AlchymeClient() {}

Rpc* AlchymeClient::rpc() {
	return m_rpc.get();
}

void AlchymeClient::Stop() {
	ScriptManager::UnInit();

	Rml::Shutdown();

	SDL_DestroyRenderer(m_sdlRenderer);
	SDL_GL_DeleteContext(m_sdlGLContext);
	SDL_DestroyWindow(m_sdlWindow);
	SDL_Quit();
}

void AlchymeClient::Start() {

	Connect("localhost", "8001");

	InitSDL();
	InitGLEW();
	InitRML();
	ScriptManager::Init();

	AlchymeGame::Start();
}



void AlchymeClient::InitSDL() {

#ifdef RMLUI_PLATFORM_WIN32
	AllocConsole();
#endif

	int window_width = 1024;
	int window_height = 768;

	SDL_Init(SDL_INIT_VIDEO);

	m_sdlWindow = SDL_CreateWindow("Alchyme",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	SDL_GLContext glcontext = SDL_GL_CreateContext(m_sdlWindow);
	int oglIdx = -1;
	int nRD = SDL_GetNumRenderDrivers();
	for (int i = 0; i < nRD; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
		{
			if (!strcmp(info.name, "opengl"))
			{
				oglIdx = i;
			}
		}
	}
	m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, oglIdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void AlchymeClient::InitGLEW() {
	GLenum err = glewInit();

	if (err != GLEW_OK)
		fprintf(stderr, "GLEW ERROR: %s\n", glewGetErrorString(err));

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glMatrixMode(GL_PROJECTION | GL_MODELVIEW);
	glLoadIdentity();

	int w = 0, h = 0;
	SDL_GetWindowSize(m_sdlWindow, &w, &h);

	//glOrtho(0, w, h, 0, 0, 1);
	glViewport(0, 0,
		w, h);

	// VSync Refresh rate
	SDL_GL_SetSwapInterval(1);
}

void AlchymeClient::InitRML() {

	m_renderInterface = std::make_unique<MyRenderInterface>(m_sdlRenderer, m_sdlWindow);
	m_systemInterface = std::make_unique<MySystemInterface>();
	m_fileInterface = std::make_unique<MyFileInterface>("./data/");

	Rml::SetRenderInterface(m_renderInterface.get());
	Rml::SetSystemInterface(m_systemInterface.get());
	Rml::SetFileInterface(m_fileInterface.get());

	if (!Rml::Initialise())
		throw std::runtime_error("Rml failed to initialize");

	int w = 0, h = 0;
	SDL_GetWindowSize(m_sdlWindow, &w, &h);

	m_rmlContext = Rml::CreateContext("default",
		Rml::Vector2i(w, h));

#ifndef NDEBUG
	Rml::Debugger::Initialise(m_rmlContext);
#endif

}



void AlchymeClient::ForwardPeerInfo(std::string username, std::string password) {
	if (serverAwaitingPeerInfo) {
		m_peer.name = username;
		m_rpc->Invoke("PeerInfo", m_version, m_peer.name, password);
		serverAwaitingPeerInfo = false;
	}
}



void AlchymeClient::Connect(std::string host, std::string port) {
	if (this->m_rpc)
		return;

	this->m_rpc = std::make_unique<Rpc>(
		std::make_shared<AsioSocket>(m_ctx));

	std::cout << "Non garbage: " << m_rpc.get()->not_garbage;

	LOG(INFO) << "Hopefully not garbage value: " << m_rpc.get()->not_garbage;

	LOG(INFO) << "2nd Hopefully not garbage value: " << m_rpc.get()->not_garbage;

	asio::ip::tcp::resolver resolver(m_ctx);
	auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);

	LOG(INFO) << "Connecting to " << host << ":" << port;

	asio::async_connect(
		m_rpc->m_socket->GetSocket(), endpoints.begin(), endpoints.end(),
		[this](const asio::error_code& ec, asio::ip::tcp::resolver::results_type::iterator it) {

		if (!ec) {
			LOG(INFO) << "Successfully connected";

			m_rpc->m_socket->Accept();

			RunTask([this] {
				ConnectCallback(m_rpc.get());
			});
		}
		else {
			if (ec == asio::error::timed_out) {
				LOG(ERROR) << "Server took too long to respond";
			}
			else if (ec == asio::error::operation_aborted) {
				LOG(ERROR) << "Connect operation aborted";
			}
			else if (ec == asio::error::not_found) {
				LOG(ERROR) << "Cannot locate host";
			}
			else if (ec.value() == 10061) { // might be win32 only
				LOG(ERROR) << "Cannot locate host (windows?)";
			}
			else {
				LOG(ERROR) << std::string("Unknown connection error: ") + ec.message();
			}

			RunTask([this]() {
				m_rpc.reset();
				AlchymeGame::StopIOThread();
			});
		}
	});

	AlchymeGame::StartIOThread();
}

void AlchymeClient::PreUpdate(float delta) {
	if (this->m_rpc)
		if (m_rpc->m_socket->Closed()) {
			DisconnectCallback(m_rpc.get());
			m_rpc.reset();
		}
		else {
			m_rpc->Update();
		}
}

void AlchymeClient::Update(float delta) {
	ScriptManager::Event::OnUpdate(delta);

	SDL_Event event;

	SDL_SetRenderDrawColor(m_sdlRenderer, 255, 255, 255, 255);
	SDL_RenderClear(m_sdlRenderer);

	m_rmlContext->Render();
	SDL_RenderPresent(m_sdlRenderer);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			Stop();
			break;

		case SDL_MOUSEMOTION:
			m_rmlContext->ProcessMouseMove(event.motion.x, event.motion.y, m_systemInterface->GetKeyModifiers());
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_rmlContext->ProcessMouseButtonDown(m_systemInterface->TranslateMouseButton(event.button.button), m_systemInterface->GetKeyModifiers());
			break;

		case SDL_MOUSEBUTTONUP:
			m_rmlContext->ProcessMouseButtonUp(m_systemInterface->TranslateMouseButton(event.button.button), m_systemInterface->GetKeyModifiers());
			break;

		case SDL_MOUSEWHEEL:
			m_rmlContext->ProcessMouseWheel(float(event.wheel.y), m_systemInterface->GetKeyModifiers());
			break;

		case SDL_KEYDOWN: {
			// Intercept F8 key stroke to toggle RmlUi's visual debugger tool
			if (event.key.keysym.sym == SDLK_F8)
			{
				Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
				break;
			}

			auto k(m_systemInterface->TranslateKey(event.key.keysym.sym));
			auto m(m_systemInterface->GetKeyModifiers());

			m_rmlContext->ProcessKeyDown(k, m);
			break;
		}
		case SDL_KEYUP: {
			auto k(m_systemInterface->TranslateKey(event.key.keysym.sym));
			auto m(m_systemInterface->GetKeyModifiers());

			m_rmlContext->ProcessKeyUp(k, m);
			break;
		}
		case SDL_TEXTINPUT: {
			m_rmlContext->ProcessTextInput(Rml::String(event.text.text));
			break;
		}
		case SDL_WINDOWEVENT: {
			switch (event.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				auto w = event.window.data1;
				auto h = event.window.data2;
				m_rmlContext->SetDimensions(Rml::Vector2i(w, h));
				break;
			}
			break;
		}

		default:
			break;
		}
	}
	m_rmlContext->Update();
}

void AlchymeClient::ConnectCallback(Rpc* rpc) {
	rpc->Register("ClientHandshake", new Method(this, &AlchymeClient::RPC_ClientHandshake));
	rpc->Register("Print", new Method(this, &AlchymeClient::RPC_Print));
	rpc->Register("PeerInfo", new Method(this, &AlchymeClient::RPC_PeerInfo));
	rpc->Register("Error", new Method(this, &AlchymeClient::RPC_Error));

	rpc->Invoke("ServerHandshake");
}

void AlchymeClient::DisconnectCallback(Rpc* rpc) {
	LOG(INFO) << "Disconnected";
}
