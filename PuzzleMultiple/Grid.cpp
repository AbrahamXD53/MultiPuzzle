#include "Grid.h"
#include <iostream>

Grid::Grid()
{
}

Grid::Grid(Texture* base, IntRect frameBox, vector<IntRect> boxes, int colorInd, Vector2i initPos, int sizeX, int sizeY, int size)
{
	colorIndex = colorInd;
	width = sizeX;
	height = sizeY;
	this->size = size;
	this->position = initPos;
	Sprite baseBox(*base, frameBox);
	this->frame = vector<Sprite>((width*height) - ((width - 2)*(height - 2)), baseBox);
	for (int i = 0; i < boxes.size(); i++)
		colors.push_back({ *base,boxes[i] });
	blocks = vector<vector<int>>(width, vector<int>(height, -1));
	tempBlocks = vector<vector<int>>(width, vector<int>(height, -1));

	//Set frame
	pFrame = frame.begin();
	FillGrid(0, 0, width, 1);
	FillGrid(0, 1, 1, height);
	FillGrid(width - 1, 1, width, height );
	FillGrid(1, height - 1, width-1, height);
}

Grid::~Grid()
{
}
Vector2f Grid::GetVector(int x, int y) {
	return Vector2f(position.x + x*size, position.y + y*size);
}
void Grid::Draw(RenderWindow * window, bool active)
{
	//Draw grid content
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (blocks[i][j] >= 0) {
				colors[colorIndex].setColor(Color::White);
				colors[blocks[i][j]].setPosition(GetVector(i,j));
				window->draw(colors[blocks[i][j]]);
			}
		}
	}
	//Draw frame
	for (vector<Sprite>::iterator it = frame.begin(); it != frame.end(); ++it)
		window->draw(*it);
	//Draw column's color
	for (int i = 1; i < width - 1; i++) {
		colors[colorIndex].setColor(Color(255, 255, 255, active?212:80));
		colors[colorIndex].setPosition(GetVector(i, height-1));
		window->draw(colors[colorIndex]);
	}
}
void Grid::Clear() {
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			tempBlocks[i][j] = -1;
		}
	}
}
void Grid::FillGrid(int x0, int y0, int x1, int y1)
{
	for (int x = x0; x < x1; x++)
		for (int y = y0; y < y1; y++)
		{
			pFrame->setPosition(position.x + (x*size), position.y + (y*size));
			pFrame++;
		}
}
void Grid::UpdateTemp(int x0, int y0, int val)
{
	tempBlocks[x0][y0] = val;
}
int Grid::GridTempValue(int x0, int y0) {
	if (x0 < 0 || y0 < 0)
		return -1;
	return tempBlocks[x0][y0];
}
void Grid::UpdateGrid(int x0, int y0, int val)
{
	blocks[x0][y0] = val;
}
int Grid::GridValue(int x0, int y0){
	if (x0 < 0 || y0 < 0)
		return -1;
	return blocks[x0][y0];
}
vector<int> Grid::GetCoords(Vector2f pos) {
	return vector<int>({ (int)((pos.x - position.x - (size / 2)) / size) , (int)((pos.y - (size / 2)) / size) });
}
vector<int> Grid::GetCoords(Vector2i pos) {
	return vector<int>({ (int)((pos.x - position.x - (size / 2)) / size) , (int)((pos.y - (size / 2)) / size) });
}
bool Grid::InsideBorder(Vector2f pos)
{
	return ((int)pos.x >= position.x + size &&
		(int)pos.x < position.x + (width*size) - size && pos.y < position.y + (height*size) - size);
}
void Grid::Flip() 
{
	//flip on y
	vector<vector<int>> temp= vector<vector<int>>(width, vector<int>(height, -1));

	for (int x = 0; x < width; x++)
		for (int y = height - 1; y >= 0; y--)
			temp[x][height-1-y]=blocks[x][y];

	//apply gravity
	vector<vector<int>> aux = vector<vector<int>>(width, vector<int>(height, -1));

	for (int x = 1; x < width-1; x++)
		for (int y = height-2,y0=height-2; y > 0; y--)
			if (temp[x][y] >= 0) {
				aux[x][y0] = temp[x][y];
					y0--;
			}
	blocks = aux;
}
void Grid::RemoveBlock(int x0, int y0) {
	blocks[x0][y0] = -1;
}
int Grid::RemovePenalty(int x0, int y0) 
{
	if (blocks[x0][y0] < 0 || blocks[x0][y0] == colorIndex)
		return -1;
	blocks[x0][y0] = -1;
	for (int y = y0; y >1; y--)
		if (blocks[x0][y] >= 0) {
			blocks[x0][y + 1] = blocks[x0][y];
			blocks[x0][y] = -1;
		}
	return DeleteFullRows();
}
void Grid::RandomPenalty(int color)
{
	int col = 1 + rand() % (width - 2);
	int row = 1;
	while (GridValue(col, row) < 0 && row<height-1)
	{
		row++;
	}
	row--;
	if (row > 0)
		if (GridValue(col, row) < 0)
			UpdateGrid(col, row, color);

}
void  Grid::DeleteRow(int y) {
	for (int x = 1; x < width-1; x++) {
		blocks[x][y] = -1;
	}
}
void  Grid::DecreaseRow(int y) {
	for (int x = 1; x < width-1; x++) {
		if (blocks[x][y]>=0) {
			blocks[x][y + 1] = blocks[x][y];
			blocks[x][y] = -1;
		}
	}
}
void  Grid::DecreaseRowsAbove(int y) {
	for (int i =y; i >1; i--)
		DecreaseRow(i);
}
bool Grid::IsRowFull(int y) {
	for (int x = 1; x < width-1; x++)
		if (blocks[x][y] != colorIndex)
			return false;
	return true;
}
int Grid::DeleteFullRows() {
	int result = 0;
	for (int y = 0; y < height; y++) {
		if (IsRowFull(y)) {
			DeleteRow(y);
			DecreaseRowsAbove(y - 1);
			--y;
			result++;
		}
	}
	return result;
}
int Grid::Evaluate() {
	return 0;
}

