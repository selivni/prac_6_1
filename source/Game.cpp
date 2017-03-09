#include "Game.hpp"

Game* iHadNoChoice;

Game::Game(): bot(), graphics(boardSizeVert, boardSizeHor)
{
	graphics.init();
	iHadNoChoice = this;
	board = new short int* [boardSizeVert];
	for (int i = 0; i < boardSizeVert; i++)
		board[i] = new short int [boardSizeHor];
	fillBoard();
	killflag = false;
	glutIdleFunc(Game_mainGameLoop);
	glutKeyboardFunc(Game_keyboardFunc);
	glutReshapeFunc(Game_reshapeFunc);
	glutDisplayFunc(Game_displayFunc);
	graphics.createBoard();
	playerPieces = aiPieces = piecesDefault;
	loopState = TURN_START;
	glutMainLoop();
}

void Game::fillBoard()
{
	bool toPut = true;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < boardSizeHor; j++)
		{
			if (toPut)
				board[i][j] = WHITE;
			else
				board[i][j] = EMPTY;
			toPut = !toPut;
		}
		toPut = !toPut;
	}
	for (int i = 3; i < boardSizeVert - 3; i++)
		for (int j = 0; j < boardSizeHor; j++)
			board[i][j] = EMPTY;
	toPut = false;
	for (int i = boardSizeVert - 3; i < boardSizeVert; i++)
	{
		for (int j = 0; j < boardSizeHor; j++)
		{
			if (toPut)
				board[i][j] = BLACK;
			else
				board[i][j] = EMPTY;
			toPut = !toPut;
		}
		toPut = !toPut;
	}
}

void Game_displayFunc()
{
	iHadNoChoice->graphics.displayFunc();
}

void Game_mouseClick(int button, int state, int x, int y)
{
	iHadNoChoice->graphics.mouseClick(button, state, x, y);
}

void Game_mouseDrag(int x, int y)
{
	iHadNoChoice->graphics.mouseDrag(x, y);
}

void Game_timerFunc(int value)
{
	iHadNoChoice->graphics.doSmoothMove();
	iHadNoChoice->mainGameLoop();
}

void Game_mainGameLoop()
{
	iHadNoChoice->mainGameLoop();
}

//only for white pieces turn
bool Game::turnCorrect()
{
	enum {
		NO_KILL,
		KILL
	};
	unsigned char uturn[5];
	unsigned char turn_[5];
	for (int i = 0; i < 5; i++)
		uturn[i] = turn[i] - '0';
	uturn[2] = NO_KILL;
	BoardParser* parser_;
	if (killflag)
		parser_ = new BoardParser(board, WHITE, lastKiller[0], lastKiller[1]);
	else
		parser_ = new BoardParser(board, WHITE);
	BoardParser& parser = *parser_;
	bool matchFlag = false;
	bool canKill = false;
	while (parser.getNextTurn(turn_))
	{
		if (!canKill && turn_[2] == KILL)
			canKill = true;
		if (!matchFlag &&
			turn_[0] == uturn[0] &&
			turn_[1] == uturn[1] &&
			turn_[3] == uturn[3] &&
			turn_[4] == uturn[4])
		{
			matchFlag = true;
			uturn[2] = turn_[2];
		}
	}
	if (uturn[2] == KILL)
		turn[2] = ':';
	else
		turn[2] = '-';
	#ifdef M_DEBUG
	cout << "matchFlag is " << matchFlag << " canKill is " << canKill 
	<< " uturn[2] is " << static_cast<int> (uturn[2]) << endl;
	#endif
	delete parser_;
	if (matchFlag && !(canKill && uturn[2] != KILL))
		return true;
	else
		return false;
/*	if ((uturn[3] - uturn[0]) * (uturn[4] - uturn[1]) == 0 ||
		 (uturn[3] - uturn[0] != uturn[4] - uturn[1] &&
		  uturn[3] - uturn[0] != uturn[1] - uturn[4]))
		return false;
	if (board[uturn[3]][uturn[4]] != EMPTY)
		return false;
	if (board[uturn[0]][uturn[1]] == WHITE)
	{
		if (uturn[3] - uturn[0] == 1)
			return true;
		if ((uturn[3] - uturn[0] == 2 ||
			 uturn[3] - uturn[0] == -2) &&
			(board[(uturn[3] + uturn[0]) / 2][(uturn[4] + uturn[1]) / 2] == BLACK ||
			board[(uturn[3] + uturn[0]) / 2][(uturn[4] + uturn[1]) / 2] == BLACK_K))
		{
			turn[2] = ':';
			return true;
		}
		else
			return false;
	}
	int signI, signJ;
	if (uturn[3] - uturn[0] > 0)
		signI = 1;
	else
		signI = -1;
	if (uturn[4] - uturn[1] > 0)
		signJ = 1;
	else
		signJ = -1;
	for (int radius = 1; uturn[0] + radius * signI != uturn[3]; radius++)
	{
		short int current =
			board[uturn[0] + radius * signI][uturn[1] + radius * signJ];
		if (current == WHITE ||
			current == WHITE_K)
			return false;
		if (current == BLACK ||
			current == BLACK_K)
		{
			turn[2] = ':';
			return true;
		}
	}
	return true;*/
}

void Game::makeTurn()
{
	unsigned char uturn[5];
	for (int i = 0; i < 5; i++)
		uturn[i] = turn[i] - '0';
	short int piece = board[uturn[0]][uturn[1]];
	short int enemy;
	bool king;
	if (piece == WHITE || piece == WHITE_K)
		enemy = BLACK;
	else
		enemy = WHITE;
	if (piece == WHITE || piece == BLACK)
		king = false;
	else
		king = true;
	if (turn[2] == ':')
	{
		if (enemy == BLACK)
			aiPieces--;
		else
			playerPieces--;
		if (!king)
		{
			killCoordinates[0] = (uturn[0] + uturn[3]) / 2;
			killCoordinates[1] = (uturn[1] + uturn[4]) / 2;
			board[killCoordinates[0]][killCoordinates[1]] = EMPTY;
		}
		else
		{
			int radius = 1;
			int signI, signJ;
			if (uturn[3] - uturn[0] > 0)
				signI = 1;
			else
				signI = -1;
			if (uturn[4] - uturn[1] > 0)
				signJ = 1;
			else
				signJ = -1;
			while (board[uturn[0] + radius * signI][uturn[1] + radius * signJ]
				   != enemy &&
				   board[uturn[0] + radius * signI][uturn[1] + radius * signJ]
				   != enemy + 1)
				radius++;
			killCoordinates[0] = uturn[0] + radius * signI;
			killCoordinates[1] = uturn[1] + radius * signJ;
			board[killCoordinates[0]][killCoordinates[1]] = EMPTY;
		}
		lastKiller[0] = uturn[3];
		lastKiller[1] = uturn[4];
	}
	board[uturn[3]][uturn[4]] = board[uturn[0]][uturn[1]];
	board[uturn[0]][uturn[1]] = EMPTY;
	if (piece == WHITE && uturn[3] == boardSizeVert - 1)
		board[uturn[3]][uturn[4]] = WHITE_K;
	if (piece == BLACK && uturn[3] == 0)
		board[uturn[3]][uturn[4]] = BLACK_K;
}

void Game::mainGameLoop()
{
	int result;
	if (loopState == TURN_START)
	{
		glutPostRedisplay();
		if ((result = gameOver(WHITE)))
		{
			cout << "Game is over for white, exiting" << endl;
			endGame();
		}
		graphics.waitHumanTurn(turn);
		glutMouseFunc(Game_mouseClick);
		glutMotionFunc(Game_mouseDrag);
		loopState = WAIT_HUMAN;
	} else if (loopState == WAIT_HUMAN)
	{
		glutPostRedisplay();
		if (graphics.turnEnded())
		{
//			cout  << endl << endl << turn[0] << turn[1] << turn[2] << turn[3] << turn[4] << endl << endl;
			if (turnCorrect())
			{
				cout << "Human turn: ";
				cout << turn[0] << turn[1] << turn[2] << turn[3] << turn[4] << endl;
				makeTurn();
				graphics.update(turn, killCoordinates);
				if (turn[2] != ':' ||
					!((board[(int)turn[3] - '0'][(int)turn[4] - '0'] == WHITE &&
					canKillNearby(turn[3] - '0',turn[4] - '0',BLACK)) ||

					(board[(int)turn[3] - '0'][(int)turn[4] - '0'] == WHITE_K &&
					canKillKingNearby(turn[3] - '0', turn[4] - '0',BLACK))))

				{
					#ifdef M_DEBUG
					cout << "White cant kill anymore blacks" << endl;
					#endif
					loopState = BOT_TURN;
					killflag = false;
					graphics.humanEnd();
					if ((result = gameOver(BLACK)))
					{
						cout << "Game is over for black, exiting" << endl;
						endGame();
					}
				} else
					killflag = true;
			} else
			{
				turn[3] = turn[0];
				turn[4] = turn[1];
				graphics.update(turn, killCoordinates);
			}
		}
	} else if (loopState == BOT_TURN)
	{
		bot.makeTurn(board, turn, killflag);
		makeTurn();
		graphics.startSmoothMove(turn, fps, blackTurnTime);
		glutIdleFunc(0);
		glutTimerFunc(1000 / fps, Game_timerFunc, 0);
		loopState = SMOOTH_MOVE;
	} else if (loopState == SMOOTH_MOVE)
	{
		if (graphics.smoothGoing())
			glutTimerFunc(1000 / fps, Game_timerFunc, 0);
		else
		{
			if (turn[2] != ':' ||
				!((board[(int)turn[3] - '0'][(int)turn[4] - '0'] == BLACK &&
				canKillNearby(turn[3] - '0',turn[4] - '0',WHITE)) ||
				(board[(int)turn[3] - '0'][(int)turn[4] - '0'] == BLACK_K &&
				canKillKingNearby(turn[3] - '0',turn[4] - '0',WHITE))))
	
			{
				#ifdef M_DEBUG
				cout << "Bot cant kill anymore" << endl;
				#endif
				killflag = false;
				loopState = TURN_START;
				glutIdleFunc(Game_mainGameLoop);
			} else
			{
				killflag = true;
				loopState = BOT_TURN;
				glutIdleFunc(Game_mainGameLoop);
			}
		}
	}
}

void Game_reshapeFunc(int newWidth, int newHeight)
{
	iHadNoChoice->graphics.reshapeFunc(newWidth, newHeight);
}

void Game_keyboardFunc(unsigned char key, int x, int y)
{
	iHadNoChoice->keyboardFunc(key, x, y);
}

void Game::keyboardFunc(unsigned char key, int x, int y)
{
	if (key == 27 || key == 'q')
	{
		graphics.shutDown();
		exit(0);
	}
/*	if (key == 'r' || key == 'R')
	{
		refreshBoard();
	}*/
}

bool Game::haveFreeForward(int i, int j)
{
	return (i < boardSizeVert - 1 &&
			((j > 0 && board[i+1][j-1] == EMPTY) ||
			 (j < boardSizeHor - 1 && board[i+1][j+1] == EMPTY)));
}

bool Game::haveFreeBackwards(int i, int j)
{
	return (i > 0 &&
			((j > 0 && board[i-1][j-1] == EMPTY) ||
			 (j < boardSizeHor - 1 && board[i-1][j+1] == EMPTY)));
}

bool Game::canKillNearby(int i, int j, short int enemy)
{
	return ((j > 1 && i < boardSizeVert - 2 && 
			(board[i+1][j-1] == enemy || board[i+1][j-1] == enemy + 1) &&
			board[i+2][j-2] == EMPTY) ||

			(j > 1 && i > 1 && 
			(board[i-1][j-1] == enemy || board[i-1][j-1] == enemy + 1) &&
			board[i-2][j-2] == EMPTY) ||

			(j < boardSizeHor - 2 && i < boardSizeVert - 2 &&
			(board[i+1][j+1] == enemy || board[i+1][j+1] == enemy + 1) &&
			board[i+2][j+2] == EMPTY) ||
			
			(j < boardSizeHor - 2 && i > 1 &&
			(board[i-1][j+1] == enemy || board[i-1][j+1] == enemy + 1) &&
			board[i-2][j+2] == EMPTY));
}

bool Game::canKillKingNearby(int i, int j, short int enemy)
{
	for (int direction = 0; direction < 4; direction++)
	{
		int signI = 1, signJ = 1;
		int radius = 1;
		bool metEnemy = false;
		if (direction == 0)
		{
			signI = -1;
			signJ = -1;
		} else if (direction == 1)
		{
			signI = -1;
			signJ = 1;
		} else if (direction == 2)
		{
			signI = 1;
			signJ = 1;
		} else if (direction == 3)
		{
			signI = 1;
			signJ = -1;
		}
		while (i + radius * signI >= 0 &&
			   i + radius * signI < boardSizeVert &&
			   j + radius * signJ >= 0 &&
			   j + radius * signJ < boardSizeHor)
		{
			short int current = 
				board[i + radius * signI][j + radius * signJ];
			if (!metEnemy)
			{
				if (current == enemy ||
					current == enemy + 1)
					metEnemy = true;
				else if (current != EMPTY)
					break;
			} else
			{
				if (current == EMPTY)
					return true;
				else
					return false;
			}
			radius++;
		}
	}
	return false;
}

// who + 1 == king
bool Game::cantMove(int who)
{
	if (who == WHITE)
	{
		int opp = BLACK;
		for (int i = 0; i < boardSizeVert; i++)
			for (int j = 0; j < boardSizeHor; j++)
			{
				if (board[i][j] == who)
				{
					if (haveFreeForward(i,j) ||
						canKillNearby(i,j, opp))
						return false;
				} else 
				if (board[i][j] == who + 1)
				{
					if (haveFreeForward(i,j) ||
						haveFreeBackwards(i,j) ||
						canKillKingNearby(i,j,opp))
						return false;
				}
			}
		return true;
	}
	if (who == BLACK)
	{
		int opp = WHITE;
		for (int i = 0; i < boardSizeVert; i++)
			for (int j = 0; j < boardSizeHor; j++)
			{
				if (board[i][j] == who)
				{
					if (haveFreeBackwards(i,j) ||
						canKillNearby(i,j,opp))
						return false;
				} else 
				if (board[i][j] == who + 1)
				{
					if (haveFreeForward(i,j) ||
						haveFreeBackwards(i,j) ||
						canKillKingNearby(i,j,opp))
						return false;
				}
			}
		return true;
	}
	return false;
}

void Game::endGame()
{
	usleep(1000000);
	exit(0);
}

int Game::gameOver(int who)
{
	if (who == BLACK)
	{
		if (aiPieces == 0 ||
			cantMove(BLACK))
			return WHITE_WON;
	} else
		if (playerPieces == 0 ||
			cantMove(WHITE))
			return BLACK_WON;
	//If i will have enough CPU when this ends i'll add some tie situations
	return GAME_NOT_OVER;
}
