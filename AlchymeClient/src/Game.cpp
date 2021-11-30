#include "Game.h"
#include <memory>
#include "Client.hpp"
#include <optick.h>

std::unique_ptr<Client> client;
GameState gameState = GameState::MainMenu;

void RunMainGame() {
	//render = std::make_unique<Render>(800, 600, "Alchyme");
	client = std::make_unique<Client>();

	auto last_tick = std::chrono::steady_clock::now();

	while (client->m_running) {
		OPTICK_FRAME("MainThread");

		const auto now = std::chrono::steady_clock::now();

		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = std::chrono::steady_clock::now();

		client->Update((float)dt / 1000000.f);
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