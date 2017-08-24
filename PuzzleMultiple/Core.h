#pragma once
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include <string>
#include "Tetromino.h"
#include <cmath>
#include <algorithm>
#include <iostream>


using namespace sf;
using namespace std;

class Core
{
public:
	Core();
	~Core();
	int Init(RenderWindow* renderWindow, int gridW,int gridH,int blockS);
	bool Update();
	void SwitchPlayer2();
	bool UpdateInput(Keyboard::Key code);
	bool UpdatePhisycs(const bool plyerOne);
	void FlipBoard();
	void DrawMirrors(bool playerOne);
	void Draw();
	void ShowGameOver();
	void CalculateMousePosition();
	void ProcessRemoval();
	void RotatePiece(bool isPlayerOne);
	void Move(int value, bool isPlayerOne);
	void NextParent(bool playerOne);
	void PrevParent(bool playerOne);
private:
	RenderWindow* window;
	Texture blocksTexture,
		selectorsTexture,
		buttonsTexture;
	Font textFont;
	Sprite selector;
	View view;
	Tetromino playerOnePiece,playerTwoPiece,nextPiece;
	Vector2i mouseCoords;
	Tetromino::TetrominoType pieceType;
	Text gameOverText;
	
	int* currentParents = new int[2]{ 0,0 };

	int score, 
		pendingRemovals,
		nextColor,
		gridWidth, gridHeight, blockSize,
		mouseGrid;

	bool botPlaying;

	float angle = 0;

	/*bool changeParent = false;
	int prevParent = currentParent;*/

	vector<IntRect> colors;
	vector<Grid> gridGroup;
	vector<Text> displayTexts;
	vector<Sprite> keySprites;
	vector<int> NextStates();
	Clock clock;


	int LoadResources();
	void NewTetromino(bool isPlayerOne);
	void UpdateParent(int prevParent, bool isPlayerOne);
};

