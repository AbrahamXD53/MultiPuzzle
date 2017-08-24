#pragma once
#include <SFML/Graphics.hpp>
#include "Grid.h"
#include <string>
#include "Tetromino.h"
#include <cmath>
#include <list>
#include <array>

using namespace sf;
using namespace std;

class Core
{
public:
	Core();
	~Core();
	int Init(RenderWindow* renderWindow, int gridW,int gridH,int blockS);
	void Update();
	bool UpdatePhisycs();
	void FlipBoard();
	void Draw();
	void ShowGameOver();
	void CalculateMousePosition();
	void ProcessRemoval();
	void RotatePiece();
	void Move(int value);
	void NextParent();
	void PrevParent();
private:
	RenderWindow* window;
	Texture blocksTexture,
		selectorsTexture,
		buttonsTexture;
	Font textFont;
	Sprite selector;
	View view;
	Tetromino currentPiece,nextPiece;
	Vector2i mouseCoords;
	Tetromino::TetrominoType pieceType;
	Text gameOverText;
	
	int score, 
		pendingRemovals,
		nextColor,
		currentParent,
		gridWidth, gridHeight, blockSize,
		mouseGrid;

	float angle = 0;

	/*bool changeParent = false;
	int prevParent = currentParent;*/

	vector<IntRect> colors;
	vector<Grid> gridGroup;
	vector<Text> displayTexts;
	vector<Sprite> keySprites;

	int LoadResources();
	void NewTetromino();
	void UpdateParent(int prevParent);
};

