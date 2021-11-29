#ifndef GAME_H
#define GAME_H

#include <optick.h>

class Client;

enum class GameState {
	MainMenu,
	Playing
};

void RunMainGame();

Client *GetClient();
GameState GetGameState();

#endif
