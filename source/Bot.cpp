#include "Bot.hpp"

#define M_DEBUG
#undef M_DEBUG
#define M_DEBUG_S
#undef M_DEBUG_S
#define M_DEBUG_M
#undef M_DEBUG_M

//const int Bot::VERT_SIZE = 8;
//const int Bot::HOR_SIZE = 8;
const int Bot::DEPTH_DEFAULT = 3;

//const int BoardParser::VERT_SIZE = 8;
//const int BoardParser::HOR_SIZE = 8;


/*
const int Tree::VERT_SIZE = 8;
const int Tree::HOR_SIZE = 8;
const int Tree::BRANCH_DEFAULT = 20;

int Tree::alpha = INT_MIN;
int Tree::beta = INT_MAX;

DynamicBranchArray::DynamicBranchArray(const DynamicBranchArray &prohibited)
	{}
void DynamicBranchArray::operator=(const DynamicBranchArray prohibited)
	{}

Tree *&DynamicBranchArray::operator[](int i)
{
	if (i >= size)
	{
		Tree** newMemory = new Tree*[size + BRANCH_DEFAULT];
		for (int j = 0; j < size; j++)
			newMemory[j] = memory[j];
		size += BRANCH_DEFAULT;
		delete[] memory;
		memory = newMemory;
	}
	return memory[i];
}

DynamicBranchArray::DynamicBranchArray(const int size_in = BRANCH_DEFAULT): size(size_in)
{
	memory = new Tree*[size];
}

Tree::Tree(const unsigned char **board_in, const int depth_in): branches(),  depth(depth_in), branchesCount(0)
{
	for (int i = 0; i < VERT_SIZE; i++)
		for (int j = 0; j < HOR_SIZE; j++)
			board[i][j] = board_in[i][j];
}

float Tree::analyze()
{
	
}
*/

BoardParser::BoardParser(short int** board_in, const unsigned char color_in,
						 unsigned char fixedI, unsigned char fixedJ):
	board(board_in), color(color_in), iteratorOne(fixedI), iteratorTwo(fixedJ),
	direction(NO_DIR), radius(0), killflag(false), fixed(true)
{
	
}

BoardParser::BoardParser(short int** board_in,
						 const unsigned char color_in):
	board(board_in), color(color_in), iteratorOne(0), iteratorTwo(0), 
	direction(NO_DIR), radius(0), killflag(false), fixed(false)
{
	
}
//At this point I realised that I dont need to use real chars like I did in Graphics 
//and Game, and can use integers in unsigned chars instead

//Also im trying to be very economical about computing, so the code will look
//a bit disgusting

bool BoardParser::getBlackTurn(unsigned char turn[5])
{
	while (iteratorOne < VERT_SIZE)
	{
		while (iteratorTwo < HOR_SIZE)
		{
			#ifdef M_DEBUG
//			unsigned char cell = board[iteratorOne][iteratorTwo];
//			if (cell != BLACK && cell != EMPTY && cell != WHITE && cell != BLACK_K &&
//				cell != WHITE_K)
//				cout << "Error: board is uninitialized: " << iteratorOne 
//					 << ' ' << iteratorTwo << endl;
			#endif
			if (board[iteratorOne][iteratorTwo] == BLACK)
				switch(direction)
				{
				case NO_DIR:
					direction = UPLEFT;
					if (iteratorOne < VERT_SIZE - 2 && iteratorTwo > 1 &&
						(board[iteratorOne + 1][iteratorTwo - 1] == WHITE ||
						 board[iteratorOne + 1][iteratorTwo - 1] == WHITE_K) &&
						board[iteratorOne + 2][iteratorTwo - 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne + 2;
						turn[4] = iteratorTwo - 2;
						return true;
					}
				case UPLEFT:
					direction = UPRIGHT;
					if (iteratorOne < VERT_SIZE - 2 &&
						iteratorTwo < HOR_SIZE - 2 &&
						(board[iteratorOne + 1][iteratorTwo + 1] == WHITE ||
						 board[iteratorOne + 1][iteratorTwo + 1] == WHITE_K) &&
						board[iteratorOne + 2][iteratorTwo + 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne + 2;
						turn[4] = iteratorTwo + 2;
						return true;
					}
				case UPRIGHT:
					direction = DOWNRIGHT;
					if (iteratorOne > 0 && iteratorTwo < HOR_SIZE - 1 &&
						board[iteratorOne - 1][iteratorTwo + 1] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = NO_KILL;
						turn[3] = iteratorOne - 1;
						turn[4] = iteratorTwo + 1;
						return true;
					}
					if (iteratorOne > 1 && iteratorTwo < HOR_SIZE - 2 &&
						(board[iteratorOne - 1][iteratorTwo + 1] == WHITE ||
						 board[iteratorOne - 1][iteratorTwo + 1] == WHITE_K) &&
						board[iteratorOne - 2][iteratorTwo + 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne - 2;
						turn[4] = iteratorTwo + 2;
						return true;
					}
				case DOWNRIGHT:
					direction = DOWNLEFT;
					if (iteratorOne > 0 && iteratorTwo > 0 &&
						board[iteratorOne - 1][iteratorTwo - 1] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = NO_KILL;
						turn[3] = iteratorOne - 1;
						turn[4] = iteratorTwo - 1;
						return true;
					}
					if (iteratorOne > 1 && iteratorTwo > 1 &&
						(board[iteratorOne - 1][iteratorTwo - 1] == WHITE ||
						 board[iteratorOne - 1][iteratorTwo - 1] == WHITE_K) &&
						 board[iteratorOne - 2][iteratorTwo - 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne - 2;
						turn[4] = iteratorTwo - 2;
						return true;
					}
				case DOWNLEFT:
				default:
					direction = NO_DIR;
				}
			else if (board[iteratorOne][iteratorTwo] == BLACK_K)
				switch(direction)
				{
				case NO_DIR:
					radius = 0;
					direction = UPLEFT;
					killflag = false;
				case UPLEFT:
					radius++;
					while (iteratorOne + radius < VERT_SIZE && iteratorTwo - radius >= 0)
					{
						if (killflag &&
							board[iteratorOne + radius][iteratorTwo - radius] != EMPTY)
							break;
						if (board[iteratorOne + radius][iteratorTwo - radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne + radius;
							turn[4] = iteratorTwo - radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo - radius] == BLACK ||
							 board[iteratorOne + radius][iteratorTwo - radius] == BLACK_K))
							break;
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo - radius] == WHITE ||
							 board[iteratorOne + radius][iteratorTwo - radius] == WHITE_K))
							killflag = true;
						radius++;
					}
					direction = UPRIGHT;
					radius = 0;
					killflag = false;
				case UPRIGHT:
					radius++;
					while (iteratorOne < VERT_SIZE - radius &&
						   iteratorTwo < HOR_SIZE - radius)
					{
						if (killflag &&
							board[iteratorOne + radius][iteratorTwo + radius] != EMPTY)
							break;
						if (board[iteratorOne + radius][iteratorTwo + radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne + radius;
							turn[4] = iteratorTwo + radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo + radius] == BLACK ||
							 board[iteratorOne + radius][iteratorTwo + radius] == BLACK_K))
							break;
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo + radius] == WHITE ||
							 board[iteratorOne + radius][iteratorTwo + radius] == WHITE_K))
							killflag = true;
						radius++;
					}
					direction = DOWNRIGHT;
					radius = 0;
					killflag = false;
				case DOWNRIGHT:
					radius++;
					while (iteratorOne >= radius && iteratorTwo < HOR_SIZE - radius)
					{
						if (killflag &&
							board[iteratorOne - radius][iteratorTwo + radius] != EMPTY)
							break;
						if (board[iteratorOne - radius][iteratorTwo + radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne - radius;
							turn[4] = iteratorTwo + radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo + radius] == BLACK ||
							 board[iteratorOne - radius][iteratorTwo + radius] == BLACK_K))
							break;
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo + radius] == WHITE ||
							 board[iteratorOne - radius][iteratorTwo + radius] == WHITE_K))
							killflag = true;
						radius++;
					}
					direction = DOWNLEFT;
					radius = 0;
					killflag = false;
				case DOWNLEFT:
					radius++;
					while (iteratorOne >= radius && iteratorTwo >= radius)
					{
						if (killflag &&
							board[iteratorOne - radius][iteratorTwo - radius] != EMPTY)
							break;
						if (board[iteratorOne - radius][iteratorTwo - radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne - radius;
							turn[4] = iteratorTwo - radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo - radius] == BLACK ||
							 board[iteratorOne - radius][iteratorTwo - radius] == BLACK_K))
							break;
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo - radius] == WHITE ||
							 board[iteratorOne - radius][iteratorTwo - radius] == WHITE_K))
							killflag = true;
						radius++;
					}
				default:
					direction = NO_DIR;
					radius = 0;
					killflag = false;
				}
			if (fixed)
				return false;
			iteratorTwo++;
		}
		iteratorTwo = 0;
		iteratorOne++;
	}
	iteratorOne = 0;
	return false;
}

bool BoardParser::getWhiteTurn(unsigned char turn[5])
{
	for (iteratorOne = iteratorOne; iteratorOne < VERT_SIZE; iteratorOne++)
	{
		while (iteratorTwo < HOR_SIZE)
		{
			if (board[iteratorOne][iteratorTwo] == WHITE)
				switch(direction)
				{
				case NO_DIR:
					direction = UPLEFT;
					if (iteratorOne < VERT_SIZE - 1 && iteratorTwo > 0 &&
						board[iteratorOne + 1][iteratorTwo - 1] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = NO_KILL;
						turn[3] = iteratorOne + 1;
						turn[4] = iteratorTwo - 1;
						return true;
					}
					if (iteratorOne < VERT_SIZE - 2 && iteratorTwo > 1 &&
						(board[iteratorOne + 1][iteratorTwo - 1] == BLACK ||
						 board[iteratorOne + 1][iteratorTwo - 1] == BLACK_K) &&
						board[iteratorOne + 2][iteratorTwo - 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne + 2;
						turn[4] = iteratorTwo - 2;
						return true;
					}
				case UPLEFT:
					direction = UPRIGHT;
					if (iteratorOne < VERT_SIZE - 1 && iteratorTwo < HOR_SIZE - 1 &&
						board[iteratorOne + 1][iteratorTwo + 1] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = NO_KILL;
						turn[3] = iteratorOne + 1;
						turn[4] = iteratorTwo + 1;
						return true;
					}
					if (iteratorOne < VERT_SIZE - 2 && iteratorTwo < HOR_SIZE - 2 &&
						(board[iteratorOne + 1][iteratorTwo + 1] == BLACK ||
						 board[iteratorOne + 1][iteratorTwo + 1] == BLACK_K) &&
						board[iteratorOne + 2][iteratorTwo + 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne + 2;
						turn[4] = iteratorTwo + 2;
						return true;
					}
				case UPRIGHT:
					direction = DOWNRIGHT;
					if (iteratorOne > 1 && iteratorTwo < HOR_SIZE - 2 &&
						(board[iteratorOne - 1][iteratorTwo + 1] == BLACK ||
						 board[iteratorOne - 1][iteratorTwo + 1] == BLACK_K) &&
						board[iteratorOne - 2][iteratorTwo + 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne - 2;
						turn[4] = iteratorTwo + 2;
						return true;
					}
				case DOWNRIGHT:
					direction = DOWNLEFT;
					if (iteratorOne > 1 && iteratorTwo > 1 &&
						(board[iteratorOne - 1][iteratorTwo - 1] == BLACK ||
						 board[iteratorOne - 1][iteratorTwo - 1] == BLACK_K) &&
						board[iteratorOne - 2][iteratorTwo - 2] == EMPTY)
					{
						turn[0] = iteratorOne;
						turn[1] = iteratorTwo;
						turn[2] = KILL;
						turn[3] = iteratorOne - 2;
						turn[4] = iteratorTwo - 2;
						return true;
					}
				case DOWNLEFT:
				default:
					direction = NO_DIR;
				}
			else if (board[iteratorOne][iteratorTwo] == WHITE_K)
				switch(direction)
				{
				case NO_DIR:
					radius = 0;
					direction = UPLEFT;
					killflag = false;
				case UPLEFT:
					radius++;
					while (iteratorOne < VERT_SIZE - radius && iteratorTwo >= radius)
					{
						if (killflag &&
							board[iteratorOne + radius][iteratorTwo - radius] != EMPTY)
							break;
						if (board[iteratorOne + radius][iteratorTwo - radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne + radius;
							turn[4] = iteratorTwo - radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo - radius] == WHITE ||
							 board[iteratorOne + radius][iteratorTwo - radius] == WHITE_K))
							break;
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo - radius] == BLACK ||
							 board[iteratorOne + radius][iteratorTwo - radius] == BLACK_K))
							killflag = true;
						radius++;
					}
					direction = UPRIGHT;
					radius = 0;
					killflag = false;
				case UPRIGHT:
					radius++;
					while (iteratorOne < VERT_SIZE - radius &&
						   iteratorTwo < HOR_SIZE - radius)
					{
						if (killflag &&
							board[iteratorOne + radius][iteratorTwo + radius] != EMPTY)
							break;
						if (board[iteratorOne + radius][iteratorTwo + radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne + radius;
							turn[4] = iteratorTwo + radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo + radius] == WHITE ||
							 board[iteratorOne + radius][iteratorTwo + radius] == WHITE_K))
							break;
						if (!killflag &&
							(board[iteratorOne + radius][iteratorTwo + radius] == BLACK ||
							 board[iteratorOne + radius][iteratorTwo + radius] == BLACK_K))
							killflag = true;
						radius++;
					}
					direction = DOWNRIGHT;
					radius = 0;
					killflag = false;
				case DOWNRIGHT:
					radius++;
					while (iteratorOne >= radius && iteratorTwo < HOR_SIZE - radius)
					{
						if (killflag &&
							board[iteratorOne - radius][iteratorTwo + radius] != EMPTY)
							break;
						if (board[iteratorOne - radius][iteratorTwo + radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne - radius;
							turn[4] = iteratorTwo + radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo + radius] == WHITE ||
							 board[iteratorOne - radius][iteratorTwo + radius] == WHITE_K))
							break;
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo + radius] == BLACK ||
							 board[iteratorOne - radius][iteratorTwo + radius] == BLACK_K))
							killflag = true;
						radius++;
					}
					direction = DOWNLEFT;
					radius = 0;
					killflag = false;
				case DOWNLEFT:
					radius++;
					while (iteratorOne >= radius && iteratorTwo >= radius)
					{
						if (killflag &&
							board[iteratorOne - radius][iteratorTwo - radius] != EMPTY)
							break;
						if (board[iteratorOne - radius][iteratorTwo - radius] == EMPTY)
						{
							turn[0] = iteratorOne;
							turn[1] = iteratorTwo;
							if (killflag)
								turn[2] = KILL;
							else
								turn[2] = NO_KILL;
							turn[3] = iteratorOne - radius;
							turn[4] = iteratorTwo - radius;
							return true;
						}
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo - radius] == WHITE ||
							 board[iteratorOne - radius][iteratorTwo - radius] == WHITE_K))
							break;
						if (!killflag &&
							(board[iteratorOne - radius][iteratorTwo - radius] == BLACK ||
							 board[iteratorOne - radius][iteratorTwo - radius] == BLACK_K))
							killflag = true;
						radius++;
					}
					direction = NO_DIR;
					radius = 0;
					killflag = false;
				default:
					break;
				}
			iteratorTwo++;
		}
		iteratorTwo = 0;
	}
	iteratorOne = 0;
	return false;
}

bool BoardParser::getNextTurn(unsigned char turn[5])
{
	if (color == WHITE)
		return getWhiteTurn(turn);
	else
		return getBlackTurn(turn);
}


void Bot::fakeMakeTurn(unsigned char turn[5], unsigned char kill[3], bool* wasKingd)
{
	unsigned char enemy;
	if (board[turn[0]][turn[1]] == BLACK ||
		board[turn[0]][turn[1]] == BLACK_K)
		enemy = WHITE;
	else
		enemy = BLACK;
	#ifdef M_DEBUG
	if (board[turn[3]][turn[4]] != EMPTY)
	{
		cout << "Error: turn given to fakeMakeTurn is wrong" << endl;
		cout << "Current board state is:" << endl;
		for (int i = VERT_SIZE - 1; i >= 0; i--)
		{
			for (int j = 0; j < HOR_SIZE; j++)
				cout << board[i][j] << ' ';
			cout << endl;
		}
		cout << "Turn given is:" << endl;
		cout << (int) turn[0];
		cout << (int) turn[1];
		if (turn[2] == KILL)
			cout << ':';
		else
			cout << '-';
		cout << (int) turn[3];
		cout << (int) turn[4];
		cout << endl;
	}
	#endif
	if (turn[2] == KILL)
	{
		short int sign1, sign2;
		if (turn[3] - turn[0] > 0)
			sign1 = 1;
		else
			sign1 = -1;
		if (turn[4] - turn[1] > 0)
			sign2 = 1;
		else
			sign2 = -1;
		int radius = 1;
		while (turn[0] + radius * sign1 >= 0 &&
			   turn[0] + radius * sign1 < VERT_SIZE &&
			   turn[1] + radius * sign2 >= 0 &&
			   turn[1] + radius * sign2 < HOR_SIZE &&
			   (board[turn[0] + radius * sign1][turn[1] + radius * sign2] != enemy &&
			    board[turn[0] + radius * sign1][turn[1] + radius * sign2] != enemy + 1))
			radius++;
		#ifdef M_DEBUG
		if (turn[0] + radius * sign1 < 0 ||
			turn[0] + radius * sign1 >= VERT_SIZE ||
			turn[1] + radius * sign2 < 0 ||
			turn[1] + radius * sign2 >= HOR_SIZE)
		{
			cout << "Error: went out of borders while trying to find what was meant to be killed" << endl;
			cout << "Current board state is:" << endl;
			for (int i = VERT_SIZE - 1; i >= 0; i--)
			{
				for (int j = 0; j < HOR_SIZE; j++)
					cout << board[i][j] << ' ';
				cout << endl;
			}
			cout << "Turn given is:" << endl;
			cout << (int) turn[0];
			cout << (int) turn[1];
			if (turn[2] == KILL)
				cout << ':';
			else
				cout << '-';
			cout << (int) turn[3];
			cout << (int) turn[4];
			cout << endl;
			cout << "sign 1: " << sign1 << endl << "sign 2: " << sign2 << endl;
			cout << "radius: " << radius << endl;
		}
		#endif
		kill[0] = turn[0] + radius * sign1;
		kill[1] = turn[1] + radius * sign2;
		if (board[turn[0] + radius * sign1][turn[1] + radius * sign2] == WHITE_K ||
			board[turn[0] + radius * sign1][turn[1] + radius * sign2] == BLACK_K)
			kill[2] = 1;
		else
			kill[2] = 0;
		board[kill[0]][kill[1]] = EMPTY;
	}

	board[turn[3]][turn[4]] = board[turn[0]][turn[1]];
	board[turn[0]][turn[1]] = EMPTY;
	if ((board[turn[3]][turn[4]] == WHITE && turn[3] == VERT_SIZE - 1) ||
		(board[turn[3]][turn[4]] == BLACK && turn[3] == 0))
	{
		*wasKingd = true;
		board[turn[3]][turn[4]]++;
	} else
		*wasKingd = false;
}

void Bot::fakeUnmakeTurn(unsigned char turn[5], unsigned char kill[3], bool wasKingd)
{
	unsigned char enemy;
	#ifdef M_DEBUG
	if (board[turn[0]][turn[1]] != EMPTY ||
		board[turn[3]][turn[4]] == EMPTY)
	{
		cout << "Error: turn given to fakeUnmakeTurn is wrong" << endl;
		cout << "REVERSE" << endl;
		cout << "Current board state is:" << endl;
		for (int i = VERT_SIZE - 1; i >= 0; i--)
		{
			for (int j = 0; j < HOR_SIZE; j++)
				cout << board[i][j] << ' ';
			cout << endl;
		}
		cout << "Turn given is:" << endl;
		cout << (int) turn[0];
		cout << (int) turn[1];
		if (turn[2] == KILL)
			cout << ':';
		else
			cout << '-';
		cout << (int) turn[3];
		cout << (int) turn[4];
		cout << endl;
		cout << "END OF REVERSE" << endl;
	}
	#endif
	if (board[turn[3]][turn[4]] == BLACK ||
		board[turn[3]][turn[4]] == BLACK_K)
		enemy = WHITE;
	else
		enemy = BLACK;
	if (turn[2] == KILL)
		board[kill[0]][kill[1]] = enemy + kill[2];
	board[turn[0]][turn[1]] = board[turn[3]][turn[4]];
	board[turn[3]][turn[4]] = EMPTY;
	if (wasKingd &&
		(board[turn[0]][turn[1]] == WHITE_K ||
		 board[turn[0]][turn[1]] == BLACK_K))
		board[turn[0]][turn[1]]--;
}
// I could have made that so heuristic function works only if there is no possibility to kill
int Bot::getHeuristic(unsigned char color, bool debugMode)
{
	BoardParser parser(board, color);
	unsigned char enemy;
	int sum = 0;
	unsigned char turn[5];
	#ifdef M_DEBUG
	unsigned char bestTurn[5];
	#endif
	int maximum = INT_MIN, minimum = INT_MAX;
	unsigned char killInfo[3];
	killInfo[0] = NO_KILL;
	killInfo[1] = killInfo[2] = 0;
	if (color == WHITE)
		enemy = BLACK;
	else
		enemy = WHITE;
	while (parser.getNextTurn(turn))
	{
		if (turn[2] == KILL)
		{
			if (killInfo[0] == NO_KILL)
				killInfo[0] = KILL;
			killInfo[1] = turn[3];
			killInfo[2] = turn[4];
			unsigned char kill[3];
			bool wasKingd = false;
			fakeMakeTurn(turn, kill, &wasKingd);
			#ifdef M_DEBUG_S
			cout << "Calling fakeMakeTurn at " << 0
				 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
			#endif

			#ifdef M_DEBUG
			unsigned char savedBoard[VERT_SIZE][HOR_SIZE];
			for (int i = 0; i < VERT_SIZE; i++)
				for (int j = 0; j < HOR_SIZE; j++)
					savedBoard[i][j] = board[i][j];
			#endif

			int value = runMinmax(1, color, INT_MIN, INT_MAX, killInfo, false);

			#ifdef M_DEBUG
			for (int i = 0; i < VERT_SIZE; i++)
				for (int j = 0; j < HOR_SIZE; j++)
					if (board[i][j] != savedBoard[i][j])
					{
						cout << "Error: minmax spoiled bot's board state and now it's incorrect!" << endl;
						cout << "On depth 0 " << "color " << static_cast<int>(color) << endl;
						cout << "Current board state is:" << endl;
						for (int i = VERT_SIZE - 1; i >= 0; i--)
						{
							for (int j = 0; j < HOR_SIZE; j++)
								cout << board[i][j] << ' ';
							cout << endl;
						}
						cout << "Saved board state is:" << endl;
						for (int i = VERT_SIZE - 1; i >= 0; i--)
						{
							for (int j = 0; j < HOR_SIZE; j++)
								cout << static_cast<int> (savedBoard[i][j]) << ' ';
							cout << endl;
						}
						exit(1);
					}
			#endif
			#ifdef M_DEBUG_S
			cout << "Calling fakeUnmakeTurn at " << 0
				 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
			#endif
			fakeUnmakeTurn(turn, kill, wasKingd);
			if (value > maximum)
			{
				#ifdef M_DEBUG
				bestTurn[0] = turn[0];
				bestTurn[1] = turn[1];
				bestTurn[2] = turn[2];
				bestTurn[3] = turn[3];
				bestTurn[4] = turn[4];
				#endif
				maximum = value;
			}
			if (value < minimum)
			{
				#ifdef M_DEBUG
				bestTurn[0] = turn[0];
				bestTurn[1] = turn[1];
				bestTurn[2] = turn[2];
				bestTurn[3] = turn[3];
				bestTurn[4] = turn[4];
				#endif
				minimum = value;
			}
		} else if (killInfo[0] == NO_KILL)
		{
			if (color == BLACK)
				sum += 1;
			else
				sum -= 1;
		}
	}
	if (killInfo[0] == KILL)
	{
		#ifdef M_DEBUG
		if (debugMode)
		{
			unsigned char kill[3];
			kill[0] = NO_KILL;
			bool wasKingd = false;
			cout << (int) bestTurn[0] << (int) bestTurn[1];
			if (bestTurn[2] == KILL)
				cout << ':';
			else
				cout << '-';
			cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
			fakeMakeTurn(bestTurn, kill, &wasKingd);
			killInfo[0] = KILL;
			if (bestTurn[2] == KILL)
			{
				killInfo[1] = bestTurn[3];
				killInfo[2] = bestTurn[4];
				runMinmax(1, color, INT_MIN, INT_MAX, killInfo, true);
			}
			fakeUnmakeTurn(bestTurn, kill, wasKingd);
		}
		#endif
		if (color == BLACK)
			return maximum;
		else
			return minimum;
	}
	for (int i = 0; i < VERT_SIZE; i++)
		for (int j = 0; j < HOR_SIZE; j++)
			if (board[i][j] == BLACK)
				sum += 20;
			else if (board[i][j] == WHITE)
				sum -= 20;
			else if (board[i][j] == BLACK_K)
				sum += 100;
			else if (board[i][j] == WHITE_K)
				sum -= 100;
	BoardParser parserTwo(board, enemy);
	while (parserTwo.getNextTurn(turn))
		if (color == BLACK)
			sum -= 1;
		else
			sum += 1;
	#ifdef M_DEBUG_S
	if (debugMode)
	{
		cout << endl;
		cout << (int)color << ' ' << sum << endl;
		cout << "Current board state is:" << endl;
		for (int i = VERT_SIZE - 1; i >= 0; i--)
		{
			for (int j = 0; j < HOR_SIZE; j++)
				cout << board[i][j] << ' ';
			cout << endl;
		}
		cout << endl;
	}
	#endif
	return sum;
}

int Bot::runMinmax(unsigned char depth, unsigned char color,
				   int alpha_, int beta_, unsigned char killInfo_[3],
				   bool debugMode)
{
	if (depth == 0)
		return getHeuristic(color, debugMode);
	
	#ifdef M_DEBUG
	unsigned char bestTurn[5];
	#endif

	int alpha = alpha_;
	int beta = beta_;

	unsigned char enemy;
	unsigned char killInfo[3];
	for (int i = 0; i < 3; i++)
		killInfo[i] = killInfo_[i];
	BoardParser* parser_;
	if (killInfo[0] == KILL)
		parser_ = new BoardParser(board, color, killInfo[1], killInfo[2]);
	else
		parser_ = new BoardParser(board, color);
	BoardParser& parser = *parser_;
//Maybe I'm being too economical
	if (color == WHITE)
	{
		enemy = BLACK;
		int minimum = INT_MAX;
		unsigned char turn[5];
		unsigned char kill[3];
		bool wasKingd = false;
		while (parser.getNextTurn(turn))
		{
			int value;
			if (turn[2] == KILL)
			{
				if (killInfo[0] == NO_KILL)
				{
					killInfo[0] = KILL;
					minimum = INT_MAX;
					alpha = alpha_;
					beta = beta_;
				}
				killInfo[1] = turn[3];
				killInfo[2] = turn[4];
				fakeMakeTurn(turn, kill, &wasKingd);
				#ifdef M_DEBUG_S
				cout << "Calling fakeMakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				#ifdef M_DEBUG
				unsigned char savedBoard[VERT_SIZE][HOR_SIZE];
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						savedBoard[i][j] = board[i][j];
				#endif
	
				value = runMinmax(depth, WHITE, alpha, beta, killInfo, false);

				#ifdef M_DEBUG
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						if (board[i][j] != savedBoard[i][j])
						{
							cout << "Error: minmax spoiled bot's board state and now it's incorrect!" << endl;
							cout << "On depth " << static_cast<int> (depth) << " color " << static_cast<int>(color) << endl;
							cout << "Current board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << board[i][j] << ' ';
								cout << endl;
							}
							cout << "Saved board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << static_cast<int> (savedBoard[i][j]) << ' ';
								cout << endl;
							}
							exit(1);
						}
				#endif
				#ifdef M_DEBUG_S
				cout << "Calling fakeUnmakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				fakeUnmakeTurn(turn, kill, wasKingd);
				if (value < minimum)
				{
					minimum = value;
					#ifdef M_DEBUG
					bestTurn[0] = turn[0];
					bestTurn[1] = turn[1];
					bestTurn[2] = turn[2];
					bestTurn[3] = turn[3];
					bestTurn[4] = turn[4];
					#endif
				}
				if (value < beta)
					beta = value;
				if (alpha > beta)
				{
					delete parser_;
					#ifdef M_DEBUG
					if (debugMode)
					{
						cout << (int) bestTurn[0] << (int) bestTurn[1];
						if (bestTurn[2] == KILL)
							cout << ':';
						else
							cout << '-';
						cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
						#ifdef M_DEBUG_M
						cout << minimum << ' ';
						#endif
						fakeMakeTurn(bestTurn, kill, &wasKingd);
						killInfo[0] = NO_KILL;
						if (bestTurn[2] == KILL)
						{
							killInfo[0] = KILL;
							killInfo[1] = bestTurn[3];
							killInfo[2] = bestTurn[4];
							runMinmax(depth, WHITE, alpha_, beta_, killInfo, true);
						}
						else
							runMinmax(depth - 1, BLACK, alpha_, beta_, killInfo, true);
						fakeUnmakeTurn(bestTurn, kill, wasKingd);
					}
					#endif
					return minimum;
				}
			} else if (killInfo[0] == NO_KILL)
			{
				fakeMakeTurn(turn, kill, &wasKingd);
				#ifdef M_DEBUG_S
				cout << "Calling fakeMakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				#ifdef M_DEBUG
				unsigned char savedBoard[VERT_SIZE][HOR_SIZE];
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						savedBoard[i][j] = board[i][j];
				#endif
	
				value = runMinmax(depth - 1, enemy, alpha, beta, killInfo, false);

				#ifdef M_DEBUG
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						if (board[i][j] != savedBoard[i][j])
						{
							cout << "Error: minmax spoiled bot's board state and now it's incorrect!" << endl;
							cout << "On depth " << static_cast<int> (depth) << " color " << static_cast<int>(color) << endl;
							cout << "Current board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << board[i][j] << ' ';
								cout << endl;
							}
							cout << "Saved board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << static_cast<int> (savedBoard[i][j]) << ' ';
								cout << endl;
							}
							exit(1);
						}
				#endif
				#ifdef M_DEBUG_S
				cout << "Calling fakeUnmakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				fakeUnmakeTurn(turn, kill, wasKingd);
				if (value < minimum)
				{
					minimum = value;
					#ifdef M_DEBUG
					bestTurn[0] = turn[0];
					bestTurn[1] = turn[1];
					bestTurn[2] = turn[2];
					bestTurn[3] = turn[3];
					bestTurn[4] = turn[4];
					#endif
				}
				if (value < beta)
					beta = value;
				if (alpha > beta)
				{
					delete parser_;
					#ifdef M_DEBUG
					if (debugMode)
					{
						cout << (int) bestTurn[0] << (int) bestTurn[1];
						if (bestTurn[2] == KILL)
							cout << ':';
						else
							cout << '-';
						cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
						#ifdef M_DEBUG_M
						cout << minimum << ' ';
						#endif
						fakeMakeTurn(bestTurn, kill, &wasKingd);
						killInfo[0] = NO_KILL;
						if (bestTurn[2] == KILL)
						{
							killInfo[0] = KILL;
							killInfo[1] = bestTurn[3];
							killInfo[2] = bestTurn[4];
							runMinmax(depth, WHITE, alpha_, beta_, killInfo, true);
						}
						else
							runMinmax(depth - 1, BLACK, alpha_, beta_, killInfo, true);
						fakeUnmakeTurn(bestTurn, kill, wasKingd);
					}
					#endif
					return minimum;
				}
			}
		}
		if (minimum == INT_MAX)
		{
			if (killInfo[0] == KILL)
			{
				killInfo[0] = NO_KILL;
				delete parser_;
				return runMinmax(depth - 1, enemy, alpha, beta, killInfo, debugMode);
			}
			else
			{
				delete parser_;
				return INT_MAX / 2;
			}
		}
		delete parser_;
		#ifdef M_DEBUG
		if (debugMode)
		{
			cout << (int) bestTurn[0] << (int) bestTurn[1];
			if (bestTurn[2] == KILL)
				cout << ':';
			else
				cout << '-';
			cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
			#ifdef M_DEBUG_M
			cout << minimum << ' ';
			#endif
			fakeMakeTurn(bestTurn, kill, &wasKingd);
			killInfo[0] = NO_KILL;
			if (bestTurn[2] == KILL)
			{
				killInfo[0] = KILL;
				killInfo[1] = bestTurn[3];
				killInfo[2] = bestTurn[4];
				runMinmax(depth, WHITE, alpha_, beta_, killInfo, true);
			}
			else
				runMinmax(depth - 1, BLACK, alpha_, beta_, killInfo, true);
			fakeUnmakeTurn(bestTurn, kill, wasKingd);
		}
		#endif
		return minimum;
	}
	else
	{
		enemy = WHITE;
		int maximum = INT_MIN;
		unsigned char turn[5];
		unsigned char kill[3];
		bool wasKingd = false;
		while (parser.getNextTurn(turn))
		{
			int value;
			if (turn[2] == KILL)
			{
				if (killInfo[0] == NO_KILL)
				{
					killInfo[0] = KILL;
					maximum = INT_MIN;
					alpha = alpha_;
					beta = beta_;
				}
				killInfo[1] = turn[3];
				killInfo[2] = turn[4];
				fakeMakeTurn(turn, kill, &wasKingd);
				#ifdef M_DEBUG_S
				cout << "Calling fakeMakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				#ifdef M_DEBUG
				unsigned char savedBoard[VERT_SIZE][HOR_SIZE];
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						savedBoard[i][j] = board[i][j];
				#endif
	
				value = runMinmax(depth, BLACK, alpha, beta, killInfo, false);

				#ifdef M_DEBUG
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						if (board[i][j] != savedBoard[i][j])
						{
							cout << "Error: minmax spoiled bot's board state and now it's incorrect!" << endl;
							cout << "On depth " << static_cast<int> (depth) << " color " << static_cast<int>(color) << endl;
							cout << "Current board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << board[i][j] << ' ';
								cout << endl;
							}
							cout << "Saved board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << static_cast<int> (savedBoard[i][j]) << ' ';
								cout << endl;
							}
							exit(1);
						}
				#endif
				#ifdef M_DEBUG_S
				cout << "Calling fakeUnmakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				fakeUnmakeTurn(turn, kill, wasKingd);
				if (value > maximum)
				{
					maximum = value;
					#ifdef M_DEBUG
					bestTurn[0] = turn[0];
					bestTurn[1] = turn[1];
					bestTurn[2] = turn[2];
					bestTurn[3] = turn[3];
					bestTurn[4] = turn[4];
					#endif
				}
				if (value > alpha)
					alpha = value;
				if (alpha > beta)
				{
					delete parser_;
					#ifdef M_DEBUG
					if (debugMode)
					{
						cout << (int) bestTurn[0] << (int) bestTurn[1];
						if (bestTurn[2] == KILL)
							cout << ':';
						else
							cout << '-';
						cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
						#ifdef M_DEBUG_M
						cout << maximum << ' ';
						#endif
						fakeMakeTurn(bestTurn, kill, &wasKingd);
						killInfo[0] = NO_KILL;
						if (bestTurn[2] == KILL)
						{
							killInfo[0] = KILL;
							killInfo[1] = bestTurn[3];
							killInfo[2] = bestTurn[4];
							runMinmax(depth, BLACK, alpha_, beta_, killInfo, true);
						}
						else
							runMinmax(depth - 1, WHITE, alpha_, beta_, killInfo, true);
						fakeUnmakeTurn(bestTurn, kill, wasKingd);
					}
					#endif
					return maximum;
				}
			} else if (killInfo[0] == NO_KILL)
			{
				fakeMakeTurn(turn, kill, &wasKingd);
				#ifdef M_DEBUG_S
				cout << "Calling fakeMakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				#ifdef M_DEBUG
				unsigned char savedBoard[VERT_SIZE][HOR_SIZE];
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						savedBoard[i][j] = board[i][j];
				#endif
	
				value = runMinmax(depth - 1, enemy, alpha, beta, killInfo, false);

				#ifdef M_DEBUG
				for (int i = 0; i < VERT_SIZE; i++)
					for (int j = 0; j < HOR_SIZE; j++)
						if (board[i][j] != savedBoard[i][j])
						{
							cout << "Error: minmax spoiled bot's board state and now it's incorrect!" << endl;
							cout << "On depth " << static_cast<int> (depth) << " color " << static_cast<int>(color) << endl;
							cout << "Current board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << board[i][j] << ' ';
								cout << endl;
							}
							cout << "Saved board state is:" << endl;
							for (int i = VERT_SIZE - 1; i >= 0; i--)
							{
								for (int j = 0; j < HOR_SIZE; j++)
									cout << static_cast<int> (savedBoard[i][j]) << ' ';
								cout << endl;
							}
							exit(1);
						}
				#endif
				#ifdef M_DEBUG_S
				cout << "Calling fakeUnmakeTurn at " << static_cast<int> (depth)
					 << " color " << static_cast<int>(color) << " king = " << wasKingd << endl;
				#endif
				fakeUnmakeTurn(turn, kill, wasKingd);
				if (value > maximum)
				{
					maximum = value;
					#ifdef M_DEBUG
					bestTurn[0] = turn[0];
					bestTurn[1] = turn[1];
					bestTurn[2] = turn[2];
					bestTurn[3] = turn[3];
					bestTurn[4] = turn[4];
					#endif
				}
				if (value > alpha)
					alpha = value;
				if (alpha > beta)
				{
					delete parser_;
					#ifdef M_DEBUG
					if (debugMode)
					{
						cout << (int) bestTurn[0] << (int) bestTurn[1];
						if (bestTurn[2] == KILL)
							cout << ':';
						else
							cout << '-';
						cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
						#ifdef M_DEBUG_M
						cout << maximum << ' ';
						#endif
						fakeMakeTurn(bestTurn, kill, &wasKingd);
						killInfo[0] = NO_KILL;
						if (bestTurn[2] == KILL)
						{
							killInfo[0] = KILL;
							killInfo[1] = bestTurn[3];
							killInfo[2] = bestTurn[4];
							runMinmax(depth, BLACK, alpha_, beta_, killInfo, true);
						}
						else
							runMinmax(depth - 1, WHITE, alpha_, beta_, killInfo, true);
						fakeUnmakeTurn(bestTurn, kill, wasKingd);
					}
					#endif
					return maximum;
				}
			}
		}
		if (maximum == INT_MIN)
		{
			if (killInfo[0] == KILL)
			{
				killInfo[0] = NO_KILL;
				delete parser_;
				return runMinmax(depth - 1, enemy, alpha, beta, killInfo, debugMode);
			}
			else
			{
				delete parser_;
				return INT_MIN / 2;
			}
		}
		delete parser_;
		#ifdef M_DEBUG
		if (debugMode)
		{
			cout << (int) bestTurn[0] << (int) bestTurn[1];
			if (bestTurn[2] == KILL)
				cout << ':';
			else
				cout << '-';
			cout << (int) bestTurn[3] << (int) bestTurn[4] << ' ';
			#ifdef M_DEBUG_M
			cout << maximum << ' ';
			#endif
			fakeMakeTurn(bestTurn, kill, &wasKingd);
			killInfo[0] = NO_KILL;
			if (bestTurn[2] == KILL)
			{
				killInfo[0] = KILL;
				killInfo[1] = bestTurn[3];
				killInfo[2] = bestTurn[4];
				runMinmax(depth, BLACK, alpha_, beta_, killInfo, true);
			}
			else
				runMinmax(depth - 1, WHITE, alpha_, beta_, killInfo, true);
			fakeUnmakeTurn(bestTurn, kill, wasKingd);
		}
		#endif
		return maximum;
	}
}

void Bot::getBest(unsigned char depth, unsigned char turn[5], bool killflag)
{
	if (depth == 0)
		return;
	int maximum = INT_MIN;
	unsigned char bestTurn[5];
	unsigned char kill[3];
	int alpha = INT_MIN, beta = INT_MAX;
	bool wasKingd = false;
	unsigned char killInfo[3];
	BoardParser* parser_;
	if (killflag)
	{
		killInfo[0] = KILL;
		parser_ = new BoardParser(board, BLACK, lastTurn[3], lastTurn[4]);
	}
	else
	{
		killInfo[0] = NO_KILL;
		parser_ = new BoardParser(board, BLACK);
	}
	BoardParser& parser = *parser_;
	killInfo[1] = 0;
	killInfo[2] = 0;
	#ifdef M_DEBUG
	cout << "Bot's board state ATM:" << endl;
	for (int i = VERT_SIZE - 1; i >= 0; i--)
	{
		for (int j = 0; j < HOR_SIZE; j++)
			cout << (int) board[i][j] << ' ';
		cout << endl;
	}
	cout << "Possible turns:" << endl;
	#endif
	while (parser.getNextTurn(turn))
	{
		int value;
		if (turn[2] == KILL)
		{
			#ifdef M_DEBUG
			cout << (int) turn[0] << (int) turn[1];
			cout << ':';
			cout << (int) turn[3] << (int) turn[4] << ' ';
			#endif
			if (killInfo[0] == NO_KILL)
			{
				killInfo[0] = KILL;
				maximum = alpha = INT_MIN;
			}
			killInfo[1] = turn[3];
			killInfo[2] = turn[4];
			fakeMakeTurn(turn, kill, &wasKingd);
			value = runMinmax(depth * 2, BLACK, alpha, beta, killInfo, true);
			#ifdef M_DEBUG
			cout << "(" << value << ")" << endl;
			#endif
			fakeUnmakeTurn(turn, kill, wasKingd);
//Obviously, maximum and alpha will always be the same value, but this function
//will run only from 1 to 10^2 times approximately, so we dont need to care too
//much about that, especially because it gives a bit better understanding of
//what's going on
			if (value > maximum)
			{
				alpha = maximum = value;
				for (int i = 0; i < 5; i++)
					bestTurn[i] = turn[i];
			}
			#ifdef M_RANDOM
			if (value == maximum)
			{
				srand(time(NULL));
				if (rand() % 100 < PERCENTAGE)
					for (int i = 0; i < 5; i++)
						bestTurn[i] = turn[i];
			}
			#endif
		} else if (killInfo[0] == NO_KILL)
		{
			#ifdef M_DEBUG
			cout << (int) turn[0] << (int) turn[1];
			cout << '-';
			cout << (int) turn[3] << (int) turn[4] << ' ';
			#endif
			fakeMakeTurn(turn, kill, &wasKingd);
			value = runMinmax(depth * 2 - 1, WHITE, alpha, beta, killInfo, true);
			#ifdef M_DEBUG
			cout << "(" << value << ")" << endl;
			#endif
			fakeUnmakeTurn(turn, kill, wasKingd);
			if (value > maximum)
			{
				alpha = maximum = value;
				for (int i = 0; i < 5; i++)
					bestTurn[i] = turn[i];
			}
			#ifdef M_RANDOM
			if (value == maximum)
			{
				srand(time(NULL));
				if (rand() % 100 < PERCENTAGE)
					for (int i = 0; i < 5; i++)
						bestTurn[i] = turn[i];
			}
			#endif
		}
	}
	if (maximum == INT_MIN)
	{
		cout << "Bot is officialy fucked and so is this program" << endl;
		turn[0] = 7;
		turn[1] = 0;
		turn[2] = NO_KILL;
		turn[3] = 7;
		turn[4] = 0;
	}
	for (int i = 0; i < 5; i++)
		turn[i] = bestTurn[i];
	delete parser_;
	if (!killflag)
	{
		if (maximum == INT_MIN / 2)
			cout << "Bot is sure he's lost" << endl;
		else if (maximum < -100)
			cout << "Bot is almost destroyed" << endl;
		else if (maximum < -30)
			cout << "Bot is having significant difficulties" << endl;
		else if (maximum < -12)
			cout << "Bot is having some troubles" << endl;
		else if (maximum < 0)
			cout << "Situation is tense, but bot feels unconfident about it" << endl;
		else if (maximum == INT_MAX / 2)
			cout << "Bot is sure he will win" << endl;
		else if (maximum > 100)
			cout << "Bot thinks he's almost destroyed you" << endl;
		else if (maximum > 30)
			cout << "Bot thinks he's giving you some huge trouble" << endl;
		else if (maximum > 12)
			cout << "Bot thinks he's giving you a bit of a trouble" << endl;
		else if (maximum > 0)
			cout << "Situation is tense, but bot feels a bit confident about it" << endl;
		else
			cout << "Bot thinks that situation is surprisingly equal" << endl;
	}
	cout << "Bot's turn: ";
}

Bot::Bot()
{
	board = new short int* [VERT_SIZE];
	for (int i = 0; i < VERT_SIZE; i++)
		board[i] = new short int[HOR_SIZE];
	bool toPut = true;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < HOR_SIZE; j++)
		{
			if (toPut)
				board[i][j] = WHITE;
			else
				board[i][j] = EMPTY;
			toPut = !toPut;
		}
		toPut = !toPut;
	}

	for (int i = 3; i < VERT_SIZE - 3; i++)
		for (int j = 0; j < HOR_SIZE; j++)
			board[i][j] = EMPTY;

	toPut = false;
	for (int i = VERT_SIZE - 3; i < VERT_SIZE; i++)
	{
		for (int j = 0; j < HOR_SIZE; j++)
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

Bot::~Bot()
{
	for (int i = 0; i < VERT_SIZE; i++)
		delete[] board[i];
	delete[] board;
}

void Bot::makeTurn(short int** board_, char turn[5], bool killflag)
{
	unsigned char uturn[5];
	unsigned char kill[3];
	bool useless = false;
	for (int i = 0; i < VERT_SIZE; i++)
		for (int j = 0; j < HOR_SIZE; j++)
			board[i][j] = board_[i][j];
	getBest(DEPTH_DEFAULT, uturn, killflag);
	for (int i = 0; i < 5; i++)
		turn[i] = uturn[i] + '0';
	if (uturn[2] == KILL)
		turn[2] = ':';
	else
		turn[2] = '-';
	fakeMakeTurn(uturn, kill, &useless);
	for (int i = 0; i < 5; i++)
		lastTurn[i] = uturn[i];
}
