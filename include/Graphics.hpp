#ifndef graphics_protector
#define graphics_protector

#define _USE_MATH_DEFINES
#include <utility>
#include <vector>
#include <list>
#include <cmath>

#include "Utility.h"
#include "SOIL.h"

using namespace std;

class Piece
{
	enum
	{
		WHITE,
		BLACK
	};
public:
	int index;
	int color;
	bool king;
	Piece(int col);
};

class Graphics
{
	Piece*** board;

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
		NO_KILL,
		KILL
	};


	int windowHeight, windowWidth;
	int offsetX, offsetY;
	int vertSize, horSize;
	float vertStep, horStep;

	int lmbState;
	bool captured;
	int capturedI;
	int capturedJ;
	bool turnDone;
	char* turn;

	bool smoothRunning;
	bool victimRemoved;
	int smoothTurnTime;
	int smoothFps;
	unsigned char smoothVictim[2];
	unsigned char smoothTurn[5];
	int smoothCounter;
	int smoothLimit;
	float smoothStepX;
	float smoothStepY;

	GLuint squaresProgram;
	GLuint squareVAO;

	GLuint piecesProgram;
	GLuint piecesVAO;

	GLuint piecesPositionsBuffer;
	GLuint piecesColorsBuffer;

	int pieceMeshSize;
	vector<VM::vec4> piecesPositions;
	vector<VM::vec4> piecesColors;

	vector<VM::vec4> generateSquareMesh();
	vector<VM::vec4> generateSquaresPositions();
	vector<VM::vec4> generateSquaresColors();

	vector<VM::vec4> generatePieceMesh();
	vector<VM::vec4> generatePiecesPositionsAndColors();

	void updatePiecesPositionsAndColors();

	GLfloat scaleVert, scaleHor;
	GLuint whiteHandler;
	GLuint blackHandler;
	GLuint whiteKHandler;
	GLuint blackKHandler;

	GLuint whiteSqrHandler;
	GLuint blackSqrHandler;

	void createSquares();
	void createPieces();
	void loadPieceTextures();
	void loadSquareTextures();
	void loadTextures();

	void drawSquares();
	void drawPieces();

	void assembleTurn(int i, int j);
	void makeTurn(unsigned char[5]);
public:
	static const int WINDOW_HEIGHT;
	static const int WINDOW_WIDTH;
	static const int POLYGONS;
	static const int PIECES;
	Graphics(int boardSizeVert, int boardSizeHor);
	void init();
	void createBoard();
	void displayFunc();
	void mouseClick(int, int, int, int);
	void mouseDrag(int, int);
	void reshapeFunc(int, int);
	void waitHumanTurn(char gameTurn[5]);
	bool turnEnded();
	void humanEnd();
	void startSmoothMove(char gameTurn[5], int, int);
	void doSmoothMove();
	void update(char gameTurn[5], unsigned char kill[2]);
	bool smoothGoing();
	void shutDown();
};

#endif
