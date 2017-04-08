#include "Graphics.hpp"

Piece::Piece(int col)
{
	king = false;
	color = col;
}

const int Graphics::WINDOW_HEIGHT = 1024;
const int Graphics::WINDOW_WIDTH = 1024;
const int Graphics::POLYGONS = 128;
const int Graphics::PIECES = 12;

Graphics::Graphics(int boardSizeVert, int boardSizeHor)
{
	board = new Piece** [boardSizeVert];
	for (int i = 0; i < boardSizeVert; i++)
	{
		board[i] = new Piece* [boardSizeHor];
		for (int j = 0; j < boardSizeHor; j++)
			board[i][j] = 0;
	}
	windowHeight = WINDOW_HEIGHT;
	windowWidth = WINDOW_WIDTH;
	offsetX = offsetY = 0;
	vertSize = boardSizeVert;
	horSize = boardSizeHor;
	vertStep = 2 / (float) boardSizeVert;
	horStep = 2 / (float) boardSizeHor;
	lmbState = 	GLUT_UP;
	captured = false;
	capturedI = capturedJ = -1;
}

void Graphics::init()
{
	int argc = 1;
	char ** argv = new char* [2];
	char prac[]="./prac1";
	argv[1] = 0;
	argv[0] = prac;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitContextVersion(3, 3);
	glutInitWindowPosition(-1, -1);
	windowHeight = WINDOW_HEIGHT;
	windowWidth = WINDOW_WIDTH;
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Checkers(shashquee)");
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	glClearColor(0.5,0.5,0.5,0.5);
	cout << "GLUT inited" << endl;

	putenv((char*)"MESA_GL_VERSION_OVERRIDE=3.3COMPAT");
	glewExperimental = true;
	glewInit();
	cout << "GLEW inited" << endl;
}

void Graphics::displayFunc()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CHECK_GL_ERRORS
	drawSquares();
	drawPieces();
	glutSwapBuffers(); CHECK_GL_ERRORS
	glDisable(GL_DEPTH_TEST);
}

void Graphics::drawSquares()
{
	glUseProgram(squaresProgram); CHECK_GL_ERRORS
	glBindVertexArray(squareVAO); CHECK_GL_ERRORS

	GLuint whiteLocation = glGetUniformLocation(squaresProgram, "whiteTex"); CHECK_GL_ERRORS
	GLuint blackLocation = glGetUniformLocation(squaresProgram, "blackTex"); CHECK_GL_ERRORS
	glUniform1i(whiteLocation, 0); CHECK_GL_ERRORS
	glUniform1i(blackLocation, 1); CHECK_GL_ERRORS
	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteSqrHandler); CHECK_GL_ERRORS
	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackSqrHandler); CHECK_GL_ERRORS

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, vertSize * horSize); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glBindVertexArray(0); CHECK_GL_ERRORS
	glUseProgram(0); CHECK_GL_ERRORS
}

// Implying this is called only when the board state is changed, so we need to update that thing every time

void Graphics::updatePiecesPositionsAndColors()
{
	vector<VM::vec4> newPiecesPositions;
	vector<VM::vec4> newPiecesColors;
	int newIndex = 0;
	for (int i = 0; i < vertSize; i++)
		for (int j = 0; j < horSize; j++)
		{
			if (board[i][j] != 0)
			{		
				newPiecesPositions.push_back(piecesPositions[board[i][j]->index]);
				if (board[i][j]->color == BLACK)
				{
					if (board[i][j]->king)
						newPiecesColors.push_back(VM::vec4(0.5, 0.2, 0.2, 1));
					else
						newPiecesColors.push_back(VM::vec4(0.2, 0.2, 0.2, 1));
				}
				if (board[i][j]->color == WHITE)
				{
					if (board[i][j]->king)
						newPiecesColors.push_back(VM::vec4(0.7, 0.7, 1, 1));
					else
						newPiecesColors.push_back(VM::vec4(0.8, 0.8, 0.8, 1));
				}
				board[i][j]->index = newIndex++;
			}
		}
	piecesPositions.clear();
	piecesColors.clear();
	piecesPositions = newPiecesPositions;
	piecesColors = newPiecesColors;
	glBindBuffer(GL_ARRAY_BUFFER, piecesPositionsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * piecesPositions.size(), piecesPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, piecesColorsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * piecesColors.size(), piecesColors.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_GL_ERRORS
}

void Graphics::drawPieces()
{
	glUseProgram(piecesProgram); CHECK_GL_ERRORS
	glBindVertexArray(piecesVAO); CHECK_GL_ERRORS
	updatePiecesPositionsAndColors();

	GLuint pieceScale = glGetUniformLocation(piecesProgram, "scaleVector"); CHECK_GL_ERRORS
	glUniform2f(pieceScale, scaleHor, scaleVert); CHECK_GL_ERRORS

	GLuint whiteLocation = glGetUniformLocation(piecesProgram, "whiteTex"); CHECK_GL_ERRORS
	GLuint blackLocation = glGetUniformLocation(piecesProgram, "blackTex"); CHECK_GL_ERRORS
	GLuint whiteKLocation = glGetUniformLocation(piecesProgram, "whiteKTex"); CHECK_GL_ERRORS
	GLuint blackKLocation = glGetUniformLocation(piecesProgram, "blackKTex"); CHECK_GL_ERRORS

	glUniform1i(whiteLocation, 0); CHECK_GL_ERRORS
	glUniform1i(blackLocation, 1); CHECK_GL_ERRORS
	glUniform1i(whiteKLocation, 2); CHECK_GL_ERRORS
	glUniform1i(blackKLocation, 3); CHECK_GL_ERRORS

//	for (int i = 0; i < piecesPositions.size(); i++)
//		cout << piecesPositions[i].x << ' ' << piecesPositions[i].y << ' ' << piecesPositions[i].z << ' ' << piecesPositions[i].w <<  endl;
//		cout << endl;

//	for (int i = 0; i < piecesColors.size(); i++)
//		cout << piecesColors[i].x << ' ' << piecesColors[i].y << ' ' << piecesColors[i].z << ' ' << endl;
//	cout << endl;
	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteHandler); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackHandler); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 2); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteKHandler); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 3); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackKHandler); CHECK_GL_ERRORS

	glDrawArraysInstanced(GL_TRIANGLES, 0, pieceMeshSize, piecesPositions.size()); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 2); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 3); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glBindVertexArray(0); CHECK_GL_ERRORS
	glUseProgram(0); CHECK_GL_ERRORS
}
//assumes that the piece has been captured before
void Graphics::assembleTurn(int i, int j)
{
/*	bool killFlag = false;
	if (i - capturedI == j - capturedJ && i - capturedI != 0)
	{
		int count = 0;
		if (i < capturedI)
			while (i + count < capturedI && !killFlag)
			{
				if (board[i + count][j + count] != 0 && 
					board[i + count][j + count]->color != board[capturedI][capturedJ]->color)
					killFlag = true;
				count++;
			}
		else 
			while(i - count > capturedI && killFlag)
			{
				if (board[i - count][j - count] != 0 &&
					board[i - count][j - count]->color != board[capturedI][capturedJ]->color)
					killFlag = true;
				count++;
			}
	}
	if (killFlag)
		turn[2] = ':';
	else */
		turn[2] = '-';
//	cout << capturedI << ' ' << capturedJ << endl;
	turn[0] = capturedI + '0';
	turn[1] = capturedJ + '0';
	turn[3] = i + '0';
	turn[4] = j + '0';
	turnDone = true;
}

void Graphics::shutDown()
{
	glutDestroyWindow(glutGetWindow());
}

void Graphics::reshapeFunc(int newWidth, int newHeight)
{
	if (newWidth < newHeight)
	{
		glViewport(0, newHeight / 2 - newWidth / 2, newWidth,
				   newWidth);
		windowWidth = windowHeight = newWidth;
		offsetY = newHeight / 2 - newWidth / 2;
		offsetX = 0;
	}
	else
	{
		glViewport(newWidth / 2 - newHeight / 2, 0,
				   newHeight, newHeight);
		windowHeight = windowWidth = newHeight;
		offsetX = newWidth / 2 - newHeight / 2;
		offsetY = 0;
	}

	cout << "Window reshaped to "<< newWidth << 'x' << newHeight << endl;
}

void Graphics::mouseDrag(int x, int y)
{
//	cout << "mouseDrag called" << endl;
	x -= offsetX;
	y -= offsetY;
	if (x >= windowWidth || y >= windowHeight || x < 0 || y < 0)
		return;
	if (captured)
	{
		float moddX = 2 * x / (float) windowWidth;
		float moddY = 2 * y / (float) windowHeight;
		if (board[capturedI][capturedJ] == 0)
		{
			cout << "Mouse drag error: piece captured doesnt exist" << endl;
			exit(1);
		}
		piecesPositions[board[capturedI][capturedJ]->index] = VM::vec4(moddX - 1, -(moddY - 1), -1.0, 0);
		glutPostRedisplay();
	}
}

void Graphics::mouseClick(int button, int state, int x, int y)
{
	x -= offsetX;
	y -= offsetY;
	if (x >= windowWidth || y >= windowHeight || x < 0 || y < 0)
		return;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !turnDone && !captured)
	{
		float moddX = 2 * x / (float) windowWidth;
		float moddY = 2 * y / (float) windowHeight;
		int i = vertSize - 1 - (int) trunc(moddY / vertStep);
		int j = (int) trunc(moddX / horStep);
		lmbState = GLUT_DOWN;
//		cout << x << ' ' << y << endl;
//		cout << i << ' ' << j << ' ' << board[i][j] << endl;
		if (board[i][j] != 0 && board[i][j]->color == WHITE)
		{
			piecesPositions[board[i][j]->index] = VM::vec4(moddX - 1,  - (moddY - 1), -0.99, 0);
			capturedI = i;
			capturedJ = j;
			captured = true;
		}
		glutPostRedisplay(); CHECK_GL_ERRORS
	} else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && captured)
	{
		float moddX = 2 * x / (float) windowWidth;
		float moddY = 2 * y / (float) windowHeight;
		int i = vertSize - 1 - (int) trunc(moddY / vertStep);
		int j = (int) trunc(moddX / horStep);
//		cout << x << ' ' << y << endl;
//		cout << i << ' ' << j << ' ' << board[i][j] << endl;
		lmbState = GLUT_UP;
		assembleTurn(i, j);
		captured = false;
	}
}

void Graphics::humanEnd()
{
	glutMouseFunc(NULL);
	glutMotionFunc(NULL);
}

bool Graphics::turnEnded()
{
	if (turnDone)
	{
		turnDone = false;
		return true;
	} else
		return turnDone;
}

void Graphics::waitHumanTurn(char gameTurn[5])
{
	turn = gameTurn;
	turnDone = false;
}

void Graphics::makeTurn(unsigned char uturn[5])
{
	short int enemy;
	if (board[uturn[0]][uturn[1]]->color == WHITE ||
		board[uturn[0]][uturn[1]]->color == WHITE_K)
		enemy = BLACK;
	else
		enemy = WHITE;
	if (uturn[2] == KILL)
	{
		short int signI, signJ;
		short int radius = 1;
		if (uturn[3] - uturn[0] > 0)
			signI = 1;
		else
			signI = -1;
		if (uturn[4] - uturn[1] > 0)
			signJ = 1;
		else
			signJ = -1;
		while (uturn[0] + radius * signI < vertSize &&
			   uturn[1] + radius * signJ < horSize &&
			   uturn[0] + radius * signI >= 0 &&
			   uturn[1] + radius * signJ >= 0 &&
			   !(board[uturn[0] + radius * signI][uturn[1] + radius * signJ] != 0	&& 
			    (board[uturn[0] + radius * signI][uturn[1] + radius * signJ]->color == enemy ||
			    board[uturn[0] + radius * signI][uturn[1] + radius * signJ]->color == enemy + 1)))
			radius++;
		#ifdef M_DEBUG
		if (uturn[0] + radius * signI == vertSize ||
			uturn[1] + radius * signJ == horSize ||
			uturn[0] + radius * signI < 0 ||
			uturn[1] + radius * signJ < 0)
		{
			cout << "Error: out of border while making bot's turn" << endl;
			cout << radius << ' ' << signI << ' ' << signJ << endl;
			exit(1);
		}
		#endif
		if (enemy == BLACK)
		{
			delete board[uturn[0] + radius * signI][uturn[1] + radius * signJ];
			board[uturn[0] + radius * signI][uturn[1] + radius * signJ] = 0;
		} else
		{
			smoothVictim[0] = uturn[0] + radius * signI;
			smoothVictim[1] = uturn[1] + radius * signJ;
		}
	}
	board[uturn[3]][uturn[4]] = board[uturn[0]][uturn[1]];
	board[uturn[0]][uturn[1]] = 0;
	if (enemy == BLACK)
	{
		piecesPositions[board[uturn[3]][uturn[4]]->index] = VM::vec4(-1 + (uturn[4] + 0.5) * horStep, -1 + (uturn[3] + 0.5) * vertStep, 0, 0);
	if ((board[uturn[3]][uturn[4]]->color == BLACK && uturn[3] == 0) ||
		(board[uturn[3]][uturn[4]]->color == WHITE && uturn[3] == vertSize - 1))
		board[uturn[3]][uturn[4]]->king = true;
	}
}

bool Graphics::smoothGoing()
{
	return smoothRunning;
}

void Graphics::doSmoothMove()
{
	if (!smoothRunning)
		return;
	VM::vec4& piecePosition = piecesPositions[board[smoothTurn[3]][smoothTurn[4]]->index];
	piecePosition = piecePosition + VM::vec4(smoothStepX, smoothStepY, 0, 0);
	smoothCounter++;
	if (smoothCounter >= smoothLimit)
	{
		Piece& piece = 
			*(board[smoothTurn[3]][smoothTurn[4]]);
		smoothRunning = false;
		piecePosition = VM::vec4(-1 + (smoothTurn[4] + 0.5) * horStep,
								 -1 + (smoothTurn[3] + 0.5) * vertStep,
								 0, 0);
		if ((piece.color == BLACK && smoothTurn[3] == 0) ||
			(piece.color == WHITE && smoothTurn[3] == vertSize - 1))
			piece.king = true;
	}
	if (smoothTurn[2] == KILL && !victimRemoved)
	{
		VM::vec4& victimPosition = 
			piecesPositions[board[smoothVictim[0]][smoothVictim[1]]->index];
		if ((smoothTurn[3] - smoothTurn[0] > 0 &&
			 victimPosition.x < piecePosition.x) ||
			(smoothTurn[3] - smoothTurn[0] < 0 &&
			 victimPosition.x > piecePosition.x))
		{
			delete board[smoothVictim[0]][smoothVictim[1]];
			board[smoothVictim[0]][smoothVictim[1]] = 0;
			victimRemoved = true;
		}
	}
	glutPostRedisplay();
}

void Graphics::startSmoothMove(char gameTurn[5], int fps, int turnTime)
{
	smoothRunning = true;
	victimRemoved = false;
	smoothFps = fps;
	smoothTurnTime = turnTime;
	smoothCounter = 0;
	smoothLimit = turnTime * fps / 1000;
	cout << gameTurn[0] << gameTurn[1] << gameTurn[2] 
		 << gameTurn[3] << gameTurn[4] << endl;
	unsigned char uturn[5];
	for (int i = 0; i < 5; i++)
		uturn[i] = turn[i] - '0';
	if (turn[2] == ':')
		uturn[2] = KILL;
	else
		uturn[2] = NO_KILL;
	piecesPositions[board[uturn[0]][uturn[1]]->index].z = -0.9;
	makeTurn(uturn);
	for (int i = 0; i < 5; i++)
		smoothTurn[i] = uturn[i];
	smoothStepX = horStep * (uturn[4] - uturn[1]) / smoothLimit;
	smoothStepY = vertStep * (uturn[3] - uturn[0]) / smoothLimit;
	glutPostRedisplay();
}

void Graphics::createBoard()
{
	createSquares();
	createPieces();
	loadTextures();
}

void Graphics::loadPieceTextures()
{
	int hsWhite, vsWhite,
		hsBlack, vsBlack,
		hsWhiteK, vsWhiteK,
		hsBlackK, vsBlackK;
	unsigned char* imageWhite = SOIL_load_image("../textures/white.png",
		&hsWhite, &vsWhite, 0, SOIL_LOAD_RGBA);
	unsigned char* imageBlack = SOIL_load_image("../textures/black.png",
		&hsBlack, &vsBlack, 0, SOIL_LOAD_RGBA);
	unsigned char* imageWhiteK = SOIL_load_image("../textures/white_king.png",
		&hsWhiteK, &vsWhiteK, 0, SOIL_LOAD_RGBA);
	unsigned char* imageBlackK = SOIL_load_image("../textures/black_king.png",
		&hsBlackK, &vsBlackK, 0, SOIL_LOAD_RGBA);
	if (imageWhite == NULL || imageBlack == NULL ||
		imageWhiteK == NULL || imageBlackK == NULL)
	{
		cout << "Error while reading texture files" << endl;
		exit(1);
	}
	glGenTextures(1, &whiteHandler); CHECK_GL_ERRORS
	glGenTextures(1, &blackHandler); CHECK_GL_ERRORS
	glGenTextures(1, &whiteKHandler); CHECK_GL_ERRORS
	glGenTextures(1, &blackKHandler); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsWhite, vsWhite, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageWhite); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageWhite);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsBlack, vsBlack, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageBlack); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageBlack);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 2); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteKHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsWhiteK, vsWhiteK, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageWhiteK); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageWhiteK);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 3); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackKHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsBlackK, vsBlackK, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageBlackK); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageBlackK);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS
	cout << "Piece textures loaded" << endl;
}

void Graphics::loadSquareTextures()
{
	int hsWhite, vsWhite,
		hsBlack, vsBlack;
	unsigned char* imageWhite = SOIL_load_image("../textures/squares/white.png",
		&hsWhite, &vsWhite, 0, SOIL_LOAD_RGBA);
	unsigned char* imageBlack = SOIL_load_image("../textures/squares/black.png",
		&hsBlack, &vsBlack, 0, SOIL_LOAD_RGBA);
	if (imageWhite == NULL || imageBlack == NULL)
	{
		cout << "Error while loading squares "
				"texture files" << endl;
		exit(1);
	}
	glGenTextures(1, &whiteSqrHandler); CHECK_GL_ERRORS
	glGenTextures(1, &blackSqrHandler); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 0); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, whiteSqrHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsWhite, vsWhite, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageWhite); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageWhite);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS

	glActiveTexture(GL_TEXTURE0 + 1); CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, blackSqrHandler); CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hsBlack, vsBlack, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageBlack); CHECK_GL_ERRORS
	glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERRORS
	SOIL_free_image_data(imageBlack);
	glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERRORS
	cout << "Square textures loaded" << endl;
}

void Graphics::loadTextures()
{
	loadPieceTextures();
	loadSquareTextures();
}

vector<VM::vec4> Graphics::generateSquareMesh()
{
	return {
		VM::vec4(0,0,0,1),
		VM::vec4(0,vertStep,0,1),
		VM::vec4(horStep,0,0,1),

		VM::vec4(0,vertStep,0,1),
		VM::vec4(horStep,0,0,1),
		VM::vec4(vertStep,horStep,0,1)
	};
}

vector<VM::vec4> Graphics::generateSquaresPositions()
{
	vector<VM::vec4> result;
	result.clear();
	for (int j = 0; j < vertSize; j++)
		for (int i = 0; i < horSize; i++)
			result.push_back(VM::vec4(i * horStep - 1, j * vertStep - 1, 1, 0));
	return result;
}

vector<VM::vec4> Graphics::generateSquaresColors()
{
	vector<VM::vec4> result;
	int color = 0;
	for (int j = 0; j < vertSize; j++)
	{
		for (int i = 0; i < horSize; i++)
		{
			result.push_back(VM::vec4(color, color, color, 1));
			++color %= 2;
		}
		++color %= 2;
	}
	return result;
}

void Graphics::createSquares()
{
	vector<VM::vec4> mesh = generateSquareMesh();
	vector<VM::vec4> positions = generateSquaresPositions();
	vector<VM::vec4> colors = generateSquaresColors();
	squaresProgram = GL::CompileShaderProgram("square");

	glGenVertexArrays(1, &squareVAO); CHECK_GL_ERRORS
	glBindVertexArray(squareVAO); CHECK_GL_ERRORS
	GLuint meshBuffer;
	glGenBuffers(1, &meshBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * mesh.size(), mesh.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint meshLocation = glGetAttribLocation(squaresProgram, "point"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(meshLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(meshLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS


	GLuint positionsBuffer;
	glGenBuffers(1, &positionsBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * positions.size(), positions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint positionsLocation = glGetAttribLocation(squaresProgram, "position"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(positionsLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(positionsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS
	glVertexAttribDivisor(positionsLocation, 1); CHECK_GL_ERRORS

	GLuint colorsBuffer;
	glGenBuffers(1, &colorsBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * colors.size(), colors.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint colorsLocation = glGetAttribLocation(squaresProgram, "color_in"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(colorsLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(colorsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS
	glVertexAttribDivisor(colorsLocation, 1); CHECK_GL_ERRORS


	glBindVertexArray(0); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_GL_ERRORS

	cout << "Squares created" << endl;
}

vector<VM::vec4> Graphics::generatePieceMesh()
{
	vector<VM::vec4> result;
	float lastSin = 0, lastCos = 1;
	for (int i = 1; i <= POLYGONS; i++)
	{
		result.push_back(VM::vec4(0,0,0,1));
		result.push_back(VM::vec4(lastCos, lastSin, 0, 1));
		lastCos = cos(2 * M_PI / POLYGONS * i);
		lastSin = sin(2 * M_PI / POLYGONS * i);
		result.push_back(VM::vec4(lastCos, lastSin, 0, 1));
	}
	return result;
}

vector<VM::vec4> Graphics::generatePiecesPositionsAndColors()
{
	vector<VM::vec4> result;
	bool toPut = true;
	int newIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < horSize; j++)
		{
			if (toPut)
			{
				board[i][j] = new Piece(WHITE);
				board[i][j]->index = newIndex++;
				piecesColors.push_back(VM::vec4(0.6, 0.6, 0.6, 1));
				piecesPositions.push_back(VM::vec4(-1 + (j + 0.5) * horStep,
										  -1 + (i + 0.5) * vertStep, 
										  0, 0));
//				cout << -1 + (j + 0.5) * horStep << ' ' << -1 + (i + 0.5) * vertStep << endl;
			} else
				board[i][j] = 0;
			toPut = !toPut;
		}
		toPut = !toPut;
	}
	
	toPut = false;

	for (int i = 3; i < vertSize - 3; i++)
		for (int j = 0; j < horSize; j++)
			board[i][j] = 0;

	for (int i = vertSize - 3; i < vertSize; i++)
	{
		for (int j = 0; j < horSize; j++)
		{
			if (toPut)
			{
				board[i][j] = new Piece(BLACK);
				board[i][j]->index = newIndex++;
				piecesColors.push_back(VM::vec4(0.2,0.2,0.2,1));
				piecesPositions.push_back(VM::vec4(-1 + (j + 0.5) * horStep,
										  -1 + (i + 0.5) * vertStep, 
										  0, 0));
			} else
				board[i][j] = 0;
			toPut = !toPut;
		}
		toPut = !toPut;
	}
	return result;
}

void Graphics::update(char gameTurn[5], unsigned char kill[2])
{
	unsigned char uturn[5];
	for (int i = 0; i < 5; i++)
		uturn[i] = gameTurn[i] - '0';
	if (gameTurn[3] == gameTurn[0] &&
		gameTurn[4] == gameTurn[1])
	{
		piecesPositions[board[uturn[0]][uturn[1]]->index] = 
			VM::vec4(-1 + (uturn[1] + 0.5) * horStep, -1 + (uturn[0] + 0.5) * vertStep,
				0, 0);
		return;
	}
	if (gameTurn[2] == ':')
	{
		delete board[kill[0]][kill[1]];
		board[kill[0]][kill[1]] = 0;
	}
	board[uturn[3]][uturn[4]] = board[uturn[0]][uturn[1]];
	board[uturn[0]][uturn[1]] = 0;
	if (board[uturn[3]][uturn[4]]->color == WHITE &&
		uturn[3] == vertSize - 1)
		board[uturn[3]][uturn[4]]->king = true;
	piecesPositions[board[uturn[3]][uturn[4]]->index] = 
										  VM::vec4(-1 + (uturn[4] + 0.5) * horStep,
										  -1 + (uturn[3] + 0.5) * vertStep, 
										  0, 0);
	capturedI = -1;
	capturedJ = -1;
	glutPostRedisplay();
}

void Graphics::createPieces()
{
	vector<VM::vec4> mesh = generatePieceMesh();
	scaleVert = vertStep * 0.8, scaleHor = horStep * 0.8;
	pieceMeshSize = mesh.size();
	generatePiecesPositionsAndColors();
	piecesProgram = GL::CompileShaderProgram("piece"); CHECK_GL_ERRORS
	glGenVertexArrays(1, &piecesVAO); CHECK_GL_ERRORS
	glBindVertexArray(piecesVAO); CHECK_GL_ERRORS

	GLuint meshBuffer;
	glGenBuffers(1, &meshBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * mesh.size(), mesh.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint meshLocation = glGetAttribLocation(piecesProgram, "point"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(meshLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(meshLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS

	glGenBuffers(1, &piecesPositionsBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, piecesPositionsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * piecesPositions.size(), piecesPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint positionsLocation = glGetAttribLocation(piecesProgram, "position"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(positionsLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(positionsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS
	glVertexAttribDivisor(positionsLocation, 1); CHECK_GL_ERRORS

	glGenBuffers(1, &piecesColorsBuffer); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, piecesColorsBuffer); CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * piecesColors.size(), piecesColors.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
	GLuint colorsLocation = glGetAttribLocation(piecesProgram, "color_in"); CHECK_GL_ERRORS
	glEnableVertexAttribArray(colorsLocation); CHECK_GL_ERRORS
	glVertexAttribPointer(colorsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERRORS
	glVertexAttribDivisor(colorsLocation, 1); CHECK_GL_ERRORS


	glBindVertexArray(0); CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_GL_ERRORS

	cout << "Pieces created" << endl;
}
