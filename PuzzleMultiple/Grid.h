#pragma once
#include <SFML\Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

class Grid
{
public:
	Grid();
	Grid(Texture * base, IntRect frameBox, vector<IntRect> boxes,int colorInd, Vector2i initPos, int sizeX, int sizeY, int size);
	~Grid();
	Vector2f GetVector(int x, int y);
	void Draw(RenderWindow * window, bool active);
	void Clear();
	bool InsideBorder(Vector2f pos);
	void Flip();
	void RemoveBlock(int x0, int y0);
	int RemovePenalty(int x0, int y0);
	void RandomPenalty(int color);
	int DeleteFullRows();
	int Evaluate();
	vector<int> GetCoords(Vector2f pos);
	vector<int> GetCoords(Vector2i pos);
	void UpdateGrid(int x0, int y0, int val);
	void UpdateTemp(int x0, int y0, int val);
	int GridTempValue(int x0, int y0);
	int GridValue(int x0, int y0);
	int colorIndex = 0;
private:
	int width;
	int height;
	int size;
	Vector2i position;
	vector<Sprite> frame;
	vector<Sprite>::iterator pFrame;
	vector<Sprite> colors;
	vector<vector<int>> blocks;
	vector<vector<int>> tempBlocks;

	void FillGrid(int x0, int y0, int x1, int y1);
	void DeleteRow(int y);
	void DecreaseRow(int y);
	void DecreaseRowsAbove(int y);
	bool IsRowFull(int y);
};

