#pragma once

class Render;
class Client;

enum class GameState {
	MainMenu,
	Playing
};

void RunMainGame();

Render *GetRender();
Client *GetClient();

GameState GetGameState();