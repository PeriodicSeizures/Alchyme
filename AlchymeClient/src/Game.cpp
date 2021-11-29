#include "Game.h"
#include <memory>
#include "Client.hpp"
#include <optick.h>

std::unique_ptr<Client> client;
GameState gameState = GameState::MainMenu;

void RunMainGame() {
	//render = std::make_unique<Render>(800, 600, "Alchyme");
	client = std::make_unique<Client>();

	while (client->m_running) {
		OPTICK_FRAME("MainThread");
		client->Update();
	}

	//render.reset();
    client.reset();
}

Client* GetClient() {
	return client.get();
}

GameState GetGameState() {
	return gameState;
}