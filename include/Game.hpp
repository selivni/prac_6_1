#ifndef gameprotector
#define gameprotector

#include <utility>
#include <unistd.h>

#include "Utility.h"
#include "Graphics.hpp"
#include "Bot.hpp"

class Game
{
	static const int piecesDefault = 12;
	static const int boardSizeVert = 8;
	static const int boardSizeHor = 8;
	static const int blackTurnTime = 300;
	static const int fps = 60;
	enum
	{
		EMPTY,
		WHITE,
		WHITE_K,
		BLACK,
		BLACK_K
	};
	enum
	{
		GAME_NOT_OVER,
		TIE,
		WHITE_WON,
		BLACK_WON
	};

	enum
	{
		IDLE,
		TURN_START,
		WAIT_HUMAN,
		BOT_TURN,
		SMOOTH_MOVE
	};

	short int** board;
	int playerPieces, aiPieces;
	Bot bot;
	char turn[5];
	unsigned char lastKiller[2];
	bool killflag;
	unsigned char killCoordinates[2];
	int gameOver(int who);
	bool haveFreeForward(int i, int j);
	bool haveFreeBackwards(int i, int j);
	bool canKillNearby(int i, int j, short int enemy);
	bool canKillKingNearby(int i, int j, short int enemy);
	bool cantMove(int who);
	bool turnCorrect();
	void fillBoard();
	void endGame();
	void makeTurn();

	int loopState;

	void refreshBoard();
public:
	Graphics graphics;
	void keyboardFunc(unsigned char key, int x, int y);
	Game();
	void mainGameLoop();
};

extern Game* iHadNoChoice;

void Game_displayFunc();
void Game_mainGameLoop();
void Game_keyboardFunc(unsigned char, int, int);
void Game_mouseClick(int, int, int, int);
void Game_mouseDrag(int, int);
void Game_reshapeFunc(int, int);
void Game_timerFunc(int);


#endif
