#include "Game.h"
#include <memory>
#include "Render.hpp"
#include "Client.hpp"

bool isRunning = true;
std::unique_ptr<Render> render;
std::unique_ptr<Client> client;
GameState gameState;

void RunMainGame() {
	render = std::make_unique<Render>(800, 600, "Alchyme");
	client = std::make_unique<Client>();

	while (isRunning) {
		client->Update();
		render->Update();
	}

	render.reset();
    client.reset();
}

Render* GetRender() {
	return render.get();
}

Client* GetClient() {
	return client.get();
}

GameState GetGameState() {
	return gameState;
}