#ifndef bot_protector
#define bot_protector

#define M_RANDOM

#ifdef M_RANDOM
#include <ctime>
#include <cstdlib>
#define PERCENTAGE 60
#endif

#include <climits>
#include <iostream>

using namespace std;

/*class DynamicBranchArray
{
	int size;
	Tree** memory;
	DynamicBranchArray(const DynamicBranchArray&);
	operator=(const DynamicBranchArray);
public:
	DynamicBranchArray(const int) 
	Tree *&operator[](int i);
};

class Tree
{
	static const int VERT_SIZE;
	static const int HOR_SIZE;
	static const int BRANCH_DEFAULT;
	unsigned char board[VERT_SIZE][HOR_SIZE];
	DynamicBranchArray branches;
	int depth;
	int branchesCount;
public:
	static int alpha, beta;
	Tree(const unsigned char **board_in, const int depth_in);
	float analyze();
};*/

class BoardParser
{
	enum {
		EMPTY,
		WHITE,
		WHITE_K,
		BLACK,
		BLACK_K
	};
	enum {
		NO_DIR,
		UPLEFT,
		UPRIGHT,
		DOWNRIGHT,
		DOWNLEFT
	};
	enum {
		NO_KILL,
		KILL
	};
	static const int VERT_SIZE = 8;
	static const int HOR_SIZE = 8;
	short int** board;

	unsigned char color;
//These represent the last state of the iterators
	unsigned char iteratorOne;
	unsigned char iteratorTwo;
	unsigned char direction;
	unsigned char radius;
//Only needed for king's turn, indicates whether or not am i searching for an empty cell
//after i attempted to kill a piece
	bool killflag;
	bool fixed;

	bool getWhiteTurn(unsigned char turn[5]);
	bool getBlackTurn(unsigned char turn[5]);

public:
	BoardParser(short int**, unsigned char, unsigned char, unsigned char);
	BoardParser(short int**, unsigned char);
	bool getNextTurn(unsigned char turn[5]);
};

class Bot
{	
	enum {
		EMPTY,
		WHITE,
		WHITE_K,
		BLACK,
		BLACK_K
	};
	enum {
		NO_KILL,
		KILL
	};
	static const int VERT_SIZE = 8;
	static const int HOR_SIZE = 8;
	static const int DEPTH_DEFAULT;
	short int** board;
	unsigned char lastTurn[5];

	void fakeMakeTurn(unsigned char[5], unsigned char[3], bool*);
	void fakeUnmakeTurn(unsigned char[5], unsigned char[3], bool);
	int getHeuristic(unsigned char, bool);
	int runMinmax(unsigned char, unsigned char, int, int, unsigned char[3], bool);
	void getBest(unsigned char depth, unsigned char[5], bool);
	void refreshMinmaxParameters();
public:
	Bot();
	~Bot();
	void makeTurn(short int**, char turn[5], bool);
};

#endif
