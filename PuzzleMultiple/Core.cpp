#include "Core.h"

Core::Core()
{
}


Core::~Core()
{
}

int Core::Init(RenderWindow * renderWindow, int gridW, int gridH, int blockS)
{
	window = renderWindow;

	blockSize = blockS;
	gridHeight = gridH;
	gridWidth = gridW;

	if (LoadResources() < 0)
		return -1;

	srand(time(NULL));

	/*Reset scoreboard*/
	score = 0;
	pendingRemovals = 0;
	currentParent = 1;

	//Set colorblocks
	colors = {
		{ 0,0,32,32 },
		{ 0,32,32,32 },
		{ 0,64,32,32 },
	};

	gridGroup = {
		{ &blocksTexture,IntRect(blockSize, 64, blockSize, blockSize), colors,1, Vector2i(0, 0),gridWidth,gridHeight, blockSize },
		{ &blocksTexture,IntRect(blockSize, 64, blockSize, blockSize), colors,0, Vector2i(blockSize*gridWidth, 0),gridWidth,gridHeight, blockSize },
		{ &blocksTexture,IntRect(blockSize, 64, blockSize, blockSize), colors,2, Vector2i(blockSize*gridWidth * 2, 0),gridWidth,gridHeight, blockSize },
	};

	//Use a view to use world positions
	view = View(FloatRect(0, 0, blockSize*gridWidth *(gridGroup.size() + 1), blockSize*gridHeight));

	selector = Sprite(selectorsTexture, IntRect(38, 0, 32, 32));

	displayTexts = {
		Text("",textFont,28),//Score
		Text("",textFont,18),//Intructions
		Text("",textFont,26),//Keys
		Text("",textFont,26)//Keys
	};

	displayTexts[0].setString("Score: "+to_string(score)+"\n\t\t\t\tNext Piece");
	displayTexts[0].setPosition(blockSize*gridWidth * gridGroup.size() + 10, 10);
	displayTexts[0].setFillColor(Color::White);

	displayTexts[1].setString("Instructions:\nPlace each piece in its respective column.\nPenalties will be applied, if a piece\nis misplaced.\n(Blocks in other columns will be spawn).\nYou can remove penalties by completing lines\nand clicking over penalty.");
	displayTexts[1].setPosition(blockSize*gridWidth * gridGroup.size() + 40, 210);
	displayTexts[1].setFillColor(Color(200,200,200));

	displayTexts[2].setString("\tRotate\n\tMove Left/Right\n\tDrop Faster\n\tSwitch column\n\n\tRemove penalty\n\nPress Escape to Quit");
	displayTexts[2].setPosition(blockSize*gridWidth * gridGroup.size() + 110, 440);
	displayTexts[2].setFillColor(Color(200, 200, 200));
	
	displayTexts[3].setFillColor(Color(80, 200, 255, 200));
	displayTexts[3].setOrigin(11*13,13);
	displayTexts[3].setPosition(blockSize*gridWidth * (gridGroup.size()+0.5f), 150);

	gameOverText = Text("",textFont,36);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(blockSize*gridWidth *(gridGroup.size() / 2.f), 300);
	

	displayTexts[3].setString("Remove penalty available");


	keySprites = {
		{ buttonsTexture,IntRect(0,0,72,72) },//w
		{ buttonsTexture,IntRect(0,72,72,72) },//A
		{ buttonsTexture,IntRect(0,144,72,72) },//S
		{ buttonsTexture,IntRect(72,0,72,72) },//D
		{ buttonsTexture,IntRect(72,144,72,72) },//E
		{ buttonsTexture,IntRect(72,72,72,72) },//Q
		{ buttonsTexture,IntRect(151,0,69,92) },//Mouse
	};
	for (int i = 0; i < keySprites.size(); i++)
		keySprites[i].setScale(0.5f, 0.5f);

	keySprites[0].setPosition(blockSize*gridWidth * gridGroup.size() + 40, 470);
	keySprites[1].setPosition(blockSize*gridWidth * gridGroup.size() + 60, 430);
	keySprites[2].setPosition(blockSize*gridWidth * gridGroup.size() + 80, 470);
	keySprites[3].setPosition(blockSize*gridWidth * gridGroup.size() + 60, 510);
	keySprites[4].setPosition(blockSize*gridWidth * gridGroup.size() + 80, 550);
	keySprites[5].setPosition(blockSize*gridWidth * gridGroup.size() + 40, 550);
	keySprites[6].setPosition(blockSize*gridWidth * gridGroup.size() + 60 ,590);

	pieceType = (Tetromino::TetrominoType)(rand() % 7);
	nextColor = rand() % gridGroup.size();
	NewTetromino();
	return 0;
}
void Core::Update() 
{
	//Place cursor over mouse grid position
	selector.setPosition(gridGroup[mouseGrid].GetVector(mouseCoords.x, mouseCoords.y));

	//simple animation
	displayTexts[3].setRotation(30 * cos(angle += .05));
	displayTexts[3].setScale(1+.2*cos(angle*2), 1 + .2*cos(angle * 2));
}
bool Core::UpdatePhisycs()
{
	currentPiece.Move(0, blockSize);
	if (!currentPiece.IsValidGridPos())
	{
		currentPiece.Move(0, -blockSize);
		int remaining = currentPiece.UpdateGrid();
		int increment = currentPiece.GetParent()->DeleteFullRows();

		score += increment;
		pendingRemovals += increment;
		displayTexts[0].setString("Score: " + to_string(score) + "\n\t\t\t\tNext Piece");

		if (remaining >= 0)
		{
			for (int i = 0; i < gridGroup.size(); i++)
			{
				if (&gridGroup[i] != currentPiece.GetParent())
					gridGroup[i].RandomPenalty(remaining);
			}
		}
		NewTetromino();
		if (!currentPiece.IsValidGridPosFinal()) //No new blocks gameover
			return false;
		return true;
	}
}
void Core::FlipBoard() {

	for (int i = 0; i < gridGroup.size(); i++)
	{
		gridGroup[i].Flip();
	}
}
void Core::Draw()
{
	window->setView(view);

	//Use current piece to draw mirrows and prediction
	//Draw in current parent
	currentPiece.Draw(window);
	//Calculate mirrors
	auto piecePos = currentPiece.GetPosition();
	auto pieceCoords = currentPiece.GetParent()->GetCoords(piecePos);
	for (int i = 0; i < gridGroup.size(); i++)
	{
		if (&gridGroup[i] != currentPiece.GetParent())
		{
			auto vectorVal = gridGroup[i].GetVector(pieceCoords[0], pieceCoords[1]);
			vectorVal.x += blockSize / 2;
			vectorVal.y += blockSize / 2;
			currentPiece.SetPosition(vectorVal);
			currentPiece.Draw(window, false);
		}
	}
	//Calculate prediction
	currentPiece.SetPosition(piecePos);
	while (currentPiece.IsValidGridPos())
		currentPiece.Move(0, blockSize);
	currentPiece.Move(0, -blockSize);
	currentPiece.Draw(window, false);

	//Reset piece
	currentPiece.SetPosition(piecePos);

	//DrawFrame
	for (int i = 0; i < gridGroup.size(); i++)
	{
		gridGroup[i].Draw(window, currentPiece.GetParent() == &gridGroup[i]);
	}

	nextPiece.Draw(window);

	//Dont display remove penalty unless we have chance
	for (int i = 0; i < displayTexts.size() - (pendingRemovals>0 ? 0 : 1); i++)
	{
		window->draw(displayTexts[i]);
	}
	for (int i = 0; i < keySprites.size(); i++)
	{
		window->draw(keySprites[i]);
	}

	window->draw(selector);

}
void Core::ShowGameOver() {
	
	gameOverText.setString("\t\tGame Over\nFinal Score: "+to_string(score)+"\nPress R to Restart");
	window->draw(gameOverText);
}
void Core::CalculateMousePosition()//Calculate mouse grid position
{
	Vector2i pixelPos = Mouse::getPosition(*window);
	Vector2f worldPos = window->mapPixelToCoords(pixelPos);
	Vector2i tempCoords = Vector2i((int)(ceil(worldPos.x / blockSize) - 1) % gridWidth, (int)(ceil(worldPos.y / blockSize) - 1) % gridHeight);
	int tempMG = ceil((ceil(worldPos.x / blockSize)) / gridWidth) - 1;
	if (tempMG >= 0 && tempMG < gridGroup.size())
		mouseGrid = tempMG;

	if (tempCoords.x >= 0 && tempCoords.x < gridWidth && tempMG < gridGroup.size())
		mouseCoords.x = tempCoords.x;
	if (tempCoords.y >= 0 && tempCoords.y < gridHeight && tempMG < gridGroup.size())
		mouseCoords.y = tempCoords.y;

}

void Core::ProcessRemoval()
{
	if (pendingRemovals > 0)
	{
		int removeResult = gridGroup[mouseGrid].RemovePenalty(mouseCoords.x, mouseCoords.y);
		if (removeResult >= 0) {
			pendingRemovals--;
			score += removeResult;
			displayTexts[0].setString("Score: " + to_string(score) + "\n\t\t\t\tNext Piece");
		}
	}
	gridGroup[mouseGrid].RemoveBlock(mouseCoords.x, mouseCoords.y);
}

void Core::RotatePiece()
{
	currentPiece.Rotate();
	if (!currentPiece.IsValidGridPos())
		currentPiece.RotateReverse();
}

void Core::Move(int value)
{
	currentPiece.Move(value, 0);
	if (!currentPiece.IsValidGridPos())
		currentPiece.Move(-value, 0);
}

void Core::NextParent()
{
	int prevParent=currentParent;
	currentParent++;
	if (currentParent >= gridGroup.size())
		currentParent = gridGroup.size() - 1;
	UpdateParent(prevParent);
}

void Core::PrevParent()
{
	int prevParent = currentParent;
	currentParent--;
	if (currentParent < 0)
		currentParent = 0;
	UpdateParent(prevParent);
}

int Core::LoadResources()
{
	if (!blocksTexture.loadFromFile("Resources/sprites.png"))
		return -1;
	if (!selectorsTexture.loadFromFile("Resources/selectors.png"))
		return -1;
	if (!buttonsTexture.loadFromFile("Resources/keys.png"))
		return -1;
	if (!textFont.loadFromFile("Resources/Dosis-Regular.ttf"))
		return -1;
	return 0;

}

void Core::NewTetromino()
{
	currentPiece={ &blocksTexture, colors[nextColor],nextColor, Vector2i((gridWidth % 2 == 0 ? -blockSize / 2 : 0) + gridWidth *(currentParent + 0.5f) * blockSize, 48), pieceType };
	currentPiece.SetParent(&gridGroup[currentParent]);
	pieceType = (Tetromino::TetrominoType)(rand() % 7);
	nextColor = rand() % gridGroup.size();
	nextPiece = { &blocksTexture, colors[nextColor],nextColor, Vector2i(blockSize*gridWidth * (gridGroup.size()+0.5f),190), pieceType };
}

void Core::UpdateParent(int prevParent)//Try to change parent if position is not valid return to prevParent
{
	auto currentPosition = currentPiece.GetPosition();
	auto pieceCoords = gridGroup[prevParent].GetCoords(currentPiece.GetPosition());
	auto vectorVal = gridGroup[currentParent].GetVector(pieceCoords[0], pieceCoords[1]);
	vectorVal.x += blockSize / 2;
	vectorVal.y += blockSize / 2;

	currentPiece.SetParent(&gridGroup[currentParent]);
	currentPiece.SetPosition(vectorVal);

	if (!currentPiece.IsValidGridPos())
	{
		currentParent = prevParent;
		currentPiece.SetParent(&gridGroup[currentParent]);
		currentPiece.SetPosition(currentPosition);
	}
}
