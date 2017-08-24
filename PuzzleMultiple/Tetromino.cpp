#include "Tetromino.h"

Tetromino::Tetromino()
{
}

Tetromino::Tetromino(Texture * texture, IntRect rect, int color, Vector2i position, TetrominoType shape)
{
	colorValue = color;
	boxes = vector<Sprite>(4, Sprite(*texture, rect));
	_shape = shape;
	size = rect.width;
	this->position = position;
	childrenPos = vector<vector<int>>(positionrules[_shape]);

	for (int i = 0; i < 4; i++) {
		boxes[i].setOrigin(size / 2, size / 2);
	}
	UpdatePos();
}

Tetromino::~Tetromino()
{
}
void Tetromino::UpdatePos()//Calculates child's relative position
{
	for (int i = 0; i < 4; i++) {
		boxes[i].setPosition((childrenPos[i][0] * size) + position.x,
			(childrenPos[i][1] * size) + position.y);
	}
}

void Tetromino::Move(int x, int y) {
	this->position.x += x;
	this->position.y += y;
	UpdatePos();
}
void Tetromino::SetPosition(Vector2f pos)
{
	this->position = Vector2i(pos.x, pos.y);
	UpdatePos();
}
void Tetromino::SetPosition(Vector2i pos)
{
	this->position = pos;
	UpdatePos();
}
Vector2i Tetromino::GetPosition() { return  this->position; }

void Tetromino::Rotate() {
	if (_shape == TetrominoType::O)
		return;
	for (int i = 0; i < 4; i++)
		childrenPos[i] = vector<int>({ childrenPos[i][1],-childrenPos[i][0] });
	UpdatePos();
}

void Tetromino::RotateReverse() {
	if (_shape == TetrominoType::O)
		return;
	for (int i = 0; i < 4; i++)
		childrenPos[i] = vector<int>({ -childrenPos[i][1],childrenPos[i][0] });
	UpdatePos();
}
void Tetromino::ClearParentInfo() {

	for (int i = 0; i < 4; i++) {
		auto v = boxes[i].getPosition();
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] >= 0 && gridpos[1] >= 0) {
			_parent->UpdateTemp(gridpos[0], gridpos[1], -1);
		}
	}
}
bool Tetromino::UpdateParentInfo(int player) {

	for (int i = 0; i < 4; i++) {
		auto v = boxes[i].getPosition();
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] >= 0 && gridpos[1] >= 0)

			if (_parent->GridTempValue(gridpos[0], gridpos[1]) > 0 && _parent->GridTempValue(gridpos[0], gridpos[1]) != player)
				return false;
	}
	for (int i = 0; i < 4; i++) {
		auto v = boxes[i].getPosition();
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] >= 0 && gridpos[1] >= 0)
			_parent->UpdateTemp(gridpos[0], gridpos[1], player);
	}
	return true;
}
bool Tetromino::IsValidTempPos(int player) {
	for (int i = 0; i < 4; i++)
	{
		auto v = boxes[i].getPosition();
		if (!_parent->InsideBorder(v))
			return false;
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] < 0)
			return false;
		if (gridpos[1] < 0)
			return true;
		if (_parent->GridTempValue(gridpos[0], gridpos[1]) > 0 && _parent->GridTempValue(gridpos[0], gridpos[1]) != player)
			return false;
	}
	return true;
}
void Tetromino::Draw(RenderWindow* window, bool active)
{
	for (int i = 0; i < 4; i++) {
		if (active)
			boxes[i].setColor(Color::White);
		else
			boxes[i].setColor(Color(255, 255, 255, 32)); //Used for mirrors and predictions

		window->draw(boxes[i]);
	}
}
void Tetromino::Draw(RenderWindow* window)
{
	for (int i = 0; i < 4; i++) {
		boxes[i].setColor(Color::White);
		window->draw(boxes[i]);
	}
}
void Tetromino::SetParent(Grid * parent)
{
	this->_parent = parent;
}

bool Tetromino::IsValidGridPosFinal() {
	for (int i = 0; i < 4; i++)
	{
		auto v = boxes[i].getPosition();
		if (!_parent->InsideBorder(v))
			return false;
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] < 0)
			return false;
		if (gridpos[1] < 0)
			return i > 0 ? true : false;
		if (_parent->GridValue(gridpos[0], gridpos[1]) >= 0)
			return false;
	}
	return true;
}

bool Tetromino::IsValidGridPos() {
	for (int i = 0; i < 4; i++)
	{
		auto v = boxes[i].getPosition();
		if (!_parent->InsideBorder(v))
			return false;
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] < 0)
			return false;
		if (gridpos[1] < 0)
			return true;
		if (_parent->GridValue(gridpos[0], gridpos[1]) >= 0)
			return false;
	}
	return true;
}
int Tetromino::GetValue() {
	int sum = 0;
	for (int i = 0; i < 4; i++)
	{
		sum += boxes[i].getPosition().y;
	}
	return sum / 4;
}
int Tetromino::UpdateGrid()
{
	if (_parent->colorIndex == colorValue)//Pice placed in correct column
	{
		for (int i = 0; i < 4; i++) {
			auto v = boxes[i].getPosition();
			auto gridpos = _parent->GetCoords(v);
			if (gridpos[0] >= 0 && gridpos[1] >= 0) {
				_parent->UpdateGrid(gridpos[0], gridpos[1], colorValue);
			}
		}
		return -1;
	}
	else {//User failed to place piece
		int i = 0;
		vector<Vector2i> checkPositions = {//Possible positions for our 4 blocks
			{ 0,-1 },
			{ 0,1 },
			{ 1,0 },
			{-1,0},
		};
		bool found = false;
		for (int j = 0; j < 2 && !found; j++)//Search to place block on ground
			for (i = 0; i < 4 && !found; i++) {
				auto v = boxes[i].getPosition();
				v.x += checkPositions[j].x*size;
				v.y += checkPositions[j].y*size;
				if (!_parent->InsideBorder(v))
				{
					found = true;
					break;
				}
			}
		if (!found) {
			for (int j = 0; j < checkPositions.size() && !found; j++)//Search to place block inline
				for (i = 0; i < 4 && !found; i++) {
					auto v = boxes[i].getPosition();
					auto gridpos = _parent->GetCoords(v);

					if (gridpos[0] >= 0 && gridpos[1] >= 0)
					{
						if (_parent->GridValue(gridpos[0] + checkPositions[j].x, gridpos[1] + checkPositions[j].y) >= 0) {
							found = true;
							break;
						}
					}
				}
		}
		//Place 1 block and Caclulate 2 block position
		auto v = boxes[i].getPosition();
		auto gridpos = _parent->GetCoords(v);
		if (gridpos[0] >= 0 && gridpos[1] >= 0) {
			_parent->UpdateGrid(gridpos[0], gridpos[1], colorValue);
			for (int j = 0; j < checkPositions.size(); j++)
			{
				if (_parent->GridValue(gridpos[0] + checkPositions[j].x, gridpos[1] + checkPositions[j].y) < 0) {
					_parent->UpdateGrid(gridpos[0] + checkPositions[j].x, gridpos[1] + checkPositions[j].y, colorValue);
					break;
				}
			}
		}
		return _parent->colorIndex;//Return colum color
	}
}

Grid* Tetromino::GetParent()
{
	return _parent;
}
