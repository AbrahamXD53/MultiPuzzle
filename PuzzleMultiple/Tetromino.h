#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Grid.h"

using namespace sf;
using namespace std;

const vector<vector<vector<int>>> positionrules = {
	{ { 0,0 },{ 0,-1 },{ -1,-1 },{ -1,0 } },
	{ { 0,0 },{ 0,-1 },{ 0,-2 },{ 0,-3 } },
	{ { 0,0 },{ 0,-1 },{ 1,-1 },{ -1,0 } },
	{ { 0,0 },{ 0,-1 },{ -1,-1 },{ 1,0 } },
	{ { 0,0 },{ 0,-1 },{ 0,-2 },{ -1,0 } },
	{ { 0,0 },{ 0,-1 },{ 0,-2 },{ 1,0 } },
	{ { 0,0 },{ 0,-1 },{ -1,-1 },{ 1,-1 } },
};

class Tetromino
{
public:
	enum TetrominoType {
		O, I, S, Z, L, J, T
	};
	Tetromino();
	Tetromino(Texture * texture, IntRect rect,int color, Vector2i position, TetrominoType shape);
	~Tetromino();
	void UpdatePos();
	void Move(int x, int y);
	void SetPosition(Vector2f pos);
	void SetPosition(Vector2i pos);
	Vector2i GetPosition();
	void Rotate();
	void RotateReverse();
	void Draw(RenderWindow* window);
	void Draw(RenderWindow * window, bool active);
	void SetParent(Grid* parent);
	bool IsValidGridPosFinal();
	bool IsValidGridPos();
	int UpdateGrid();
	bool fixed = false;
	Grid* GetParent();
private:
	Grid* _parent;
	Vector2i position;
	vector<Sprite> boxes;
	TetrominoType _shape;
	int size;
	int colorValue;
	vector<vector<int>> childrenPos;
};

