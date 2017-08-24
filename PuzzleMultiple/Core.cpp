#include "Core.h"

Core::Core()
{
}


Core::~Core()
{
}

int Core::Init(RenderWindow * renderWindow, int gridW, int gridH, int blockS)
{
	free(currentParents);
	clock = Clock();
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
	currentParents = new int[2]{ 0, 2 };

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

	displayTexts[0].setString("Score: " + to_string(score) + "\n\t\t\t\tNext Piece");
	displayTexts[0].setPosition(blockSize*gridWidth * gridGroup.size() + 10, 10);
	displayTexts[0].setFillColor(Color::White);

	displayTexts[1].setString("Instructions:\nPlace each piece in its respective column.\nPenalties will be applied, if a piece\nis misplaced.\n(Blocks in other columns will be spawn).\nYou can remove penalties by completing lines\nand clicking over penalty.");
	displayTexts[1].setPosition(blockSize*gridWidth * gridGroup.size() + 40, 210);
	displayTexts[1].setFillColor(Color(200, 200, 200));

	displayTexts[2].setString("\tRotate\n\tMove Left/Right\n\tDrop Faster\n\tSwitch column\n\n\tRemove penalty\n\nPress Escape to Quit");
	displayTexts[2].setPosition(blockSize*gridWidth * gridGroup.size() + 110, 440);
	displayTexts[2].setFillColor(Color(200, 200, 200));

	displayTexts[3].setFillColor(Color(80, 200, 255, 200));
	displayTexts[3].setOrigin(11 * 13, 13);
	displayTexts[3].setPosition(blockSize*gridWidth * (gridGroup.size() + 0.5f), 150);

	gameOverText = Text("", textFont, 36);
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
	keySprites[6].setPosition(blockSize*gridWidth * gridGroup.size() + 60, 590);

	pieceType = (Tetromino::TetrominoType)(rand() % 7);
	nextColor = rand() % gridGroup.size();
	NewTetromino(true);
	NewTetromino(false);
	return 0;
}

bool CompreStates(vector<int> i, vector<int> j) { return (i[2] > j[2]); }

vector<int> Core::NextStates() {
	vector<vector<int>> rightRotation;

	for (int i = 0; i < 4; i++)
	{
		for (int r = 0; r < i; r++)
		{
			playerTwoPiece.Rotate();
		}
		for (int x = 0; x < (gridWidth) / 2; x++)
		{
			Tetromino temp = playerTwoPiece;
			int xres = 0;
			
			for (int x0 = 0; x0 <= x && temp.IsValidGridPos(); x0++)
			{
				temp.ClearParentInfo();
				xres = x0;
				temp.Move(-blockSize, 0);
			}
			temp.Move(blockSize, 0);
			while (temp.IsValidGridPos()) {
				temp.Move(0, blockSize);
			}
			temp.Move(0, -blockSize);
			rightRotation.push_back(vector<int>({ i,-xres,temp.GetValue() }));
		}
		for (int x = 0; x < (gridWidth - 2) / 2; x++)
		{
			Tetromino temp = playerTwoPiece;

			int xres = 0;
			for (int x0 = 0; x0 <= x && temp.IsValidGridPos(); x0++)
			{
				xres = x0;
				temp.Move(blockSize, 0);
			}
			temp.Move(-blockSize, 0);
			while (temp.IsValidGridPos()) {
				temp.Move(0, blockSize);
			}
			temp.Move(0, -blockSize);
			rightRotation.push_back(vector<int>({ i,xres,temp.GetValue() }));
		}
		for (int r = 0; r < i; r++)
		{
			playerTwoPiece.RotateReverse();
		}
	}
	sort(rightRotation.begin(), rightRotation.end(), CompreStates);
	cout << "Next staes" << endl;
	for (int i = 0; i < rightRotation.size(); i++)
	{
		cout << rightRotation[i][0] << "\t" << rightRotation[i][1] << "\t" << rightRotation[i][2] << endl;
	}
	cout << endl << endl;
	return rightRotation.size() > 0 ? rightRotation[0] : vector<int>({ 0,0,0 });
}
bool AIready = false;
bool Core::Update()
{
	playerOnePiece.ClearParentInfo();
	playerOnePiece.UpdateParentInfo(1);

	//playerTwoPiece.ClearParentInfo();
	//playerTwoPiece.UpdateParentInfo(2);

	//Place cursor over mouse grid position
	selector.setPosition(gridGroup[mouseGrid].GetVector(mouseCoords.x, mouseCoords.y));

	//simple animation
	displayTexts[3].setRotation(30 * cos(angle += .05));
	displayTexts[3].setScale(1 + .2*cos(angle * 2), 1 + .2*cos(angle * 2));

	//update bot
	if (botPlaying && !AIready)
	{
		//Select right column
		int tries = 2;
		while (playerTwoPiece.colorValue != gridGroup[currentParents[1]].colorIndex && tries > 0) {
			NextParent(false);
			tries--;
		}
		tries = 2;
		while (playerTwoPiece.colorValue != gridGroup[currentParents[1]].colorIndex && tries > 0) {
			PrevParent(false);
			tries--;
		}
		auto result = NextStates();
		for (int r = 0; r < result[0]; r++)
			playerTwoPiece.Rotate();
		for (int x = 0; x < abs(result[1]); x++)
			playerTwoPiece.Move(blockSize*(result[1] > 0 ? 1 : -1), 0);
		AIready = true;
	}

	if (clock.getElapsedTime().asMilliseconds() >= 300 && botPlaying) {
		clock.restart();
		return UpdatePhisycs(false);
	}
	return true;
}

void Core::SwitchPlayer2() {
	botPlaying = !botPlaying;
	displayTexts[0].setString("Score: " + to_string(score) + "\n\t\t\t\tNext Piece");
}

bool Core::UpdateInput(Keyboard::Key code)
{
	//Input
	if (code == Keyboard::W)
		RotatePiece(true);
	if (code == Keyboard::Numpad8 && !botPlaying)
		RotatePiece(false);
	if (code == Keyboard::A)
		Move(-blockSize, true);
	if (code == Keyboard::Numpad4 && !botPlaying)
		Move(-blockSize, false);
	if (code == Keyboard::D)
		Move(blockSize, true);
	if (code == Keyboard::Numpad6 && !botPlaying)
		Move(blockSize, false);



	if (code == Keyboard::E)
		NextParent(true);
	if (code == Keyboard::Numpad3 && !botPlaying)
		NextParent(false);
	if (code == Keyboard::Q)
		PrevParent(true);
	if (code == Keyboard::Numpad1 && !botPlaying)
		PrevParent(false);

	if (code == Keyboard::S)
		return UpdatePhisycs(true);
	if (code == Keyboard::Numpad2 && !botPlaying)
		return UpdatePhisycs(false);
	return true;
}

bool Core::UpdatePhisycs(const bool playerOne = true)//Gravity
{
	auto piece = playerOne ? &playerOnePiece : &playerTwoPiece;
	piece->ClearParentInfo();
	piece->Move(0, blockSize);
	if (!piece->UpdateParentInfo(playerOne ? 1 : 2) && piece->IsValidGridPos()) {
		piece->Move(0, -blockSize);
	}
	else
	if (!piece->IsValidGridPos())
	{
		piece->ClearParentInfo();
		AIready = false;
		piece->Move(0, -blockSize);
		int remaining = piece->UpdateGrid();
		int increment = piece->GetParent()->DeleteFullRows();

		score += increment;
		pendingRemovals += increment;
		displayTexts[0].setString("Score: " + to_string(score) + "\n\t\t\t\tNext Piece");

		if (remaining >= 0)
		{
			for (int i = 0; i < gridGroup.size(); i++)
			{
				if (&gridGroup[i] != piece->GetParent())
					gridGroup[i].RandomPenalty(remaining);
			}
		}
		NewTetromino(playerOne);
		if (!piece->IsValidGridPosFinal()) //No new blocks gameover
			return false;
	}
	return true;
}
void Core::FlipBoard() {//FlipBoard

	for (int i = 0; i < gridGroup.size(); i++)
	{
		gridGroup[i].Flip();
	}
}

void Core::DrawMirrors(bool playerOne) {
	auto piece = playerOne ? &playerOnePiece : &playerTwoPiece;
	auto piecePos = piece->GetPosition();
	auto pieceCoords = piece->GetParent()->GetCoords(piecePos);
	for (int i = 0; i < gridGroup.size(); i++)
	{
		if (&gridGroup[i] != piece->GetParent())
		{
			auto vectorVal = gridGroup[i].GetVector(pieceCoords[0], pieceCoords[1]);
			vectorVal.x += blockSize / 2;
			vectorVal.y += blockSize / 2;
			piece->SetPosition(vectorVal);
			piece->Draw(window, false);
		}
	}
	//Calculate prediction
	piece->SetPosition(piecePos);
	while (piece->IsValidGridPos())
		piece->Move(0, blockSize);
	piece->Move(0, -blockSize);
	piece->Draw(window, false);

	piece->SetPosition(piecePos);
}
void Core::Draw()
{
	window->setView(view);

	//Use current piece to draw mirrows and prediction
	//Draw in current parent
	playerOnePiece.Draw(window);
	//Calculate mirrors

	DrawMirrors(true);
	playerTwoPiece.Draw(window);
	if (!botPlaying)
		DrawMirrors(false);

	//DrawFrame
	for (int i = 0; i < gridGroup.size(); i++)
	{
		gridGroup[i].Draw(window, playerOnePiece.GetParent() == &gridGroup[i] || playerTwoPiece.GetParent() == &gridGroup[i]);
	}

	nextPiece.Draw(window);

	//Dont display remove penalty unless we have chance
	for (int i = 0; i < displayTexts.size() - (pendingRemovals > 0 ? 0 : 1); i++)
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

	gameOverText.setString("\t\tGame Over\nFinal Score: " + to_string(score) + "\nPress R to Restart");
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

void Core::ProcessRemoval()//Remove Block
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

void Core::RotatePiece(bool isPlayerOne)
{
	if (isPlayerOne)
		AIready = false;
	auto piece = isPlayerOne ? &playerOnePiece : &playerTwoPiece;
	piece->ClearParentInfo();
	piece->Rotate();
	if (!piece->UpdateParentInfo(isPlayerOne ? 1 : 2) || !piece->IsValidGridPos())
		piece->RotateReverse();
}

void Core::Move(int value, bool isPlayerOne)
{
	if (isPlayerOne)
		AIready = false;
	auto piece = isPlayerOne ? &playerOnePiece : &playerTwoPiece;
	piece->ClearParentInfo();
	piece->Move(value, 0);
	if (!piece->IsValidGridPos() || !piece->UpdateParentInfo(isPlayerOne ? 1 : 2))
		piece->Move(-value, 0);


}

void Core::NextParent(bool playerOne)
{
	if (playerOne)
		AIready = false;
	int prevParent = currentParents[playerOne ? 0 : 1];
	currentParents[playerOne ? 0 : 1]++;
	if (currentParents[playerOne ? 0 : 1] >= gridGroup.size())
		currentParents[playerOne ? 0 : 1] = gridGroup.size() - 1;
	UpdateParent(prevParent, playerOne);
}

void Core::PrevParent(bool playerOne)
{
	if (playerOne)
		AIready = false;
	int prevParent = currentParents[playerOne ? 0 : 1];
	currentParents[playerOne ? 0 : 1]--;
	if (currentParents[playerOne ? 0 : 1] < 0)
		currentParents[playerOne ? 0 : 1] = 0;
	UpdateParent(prevParent, playerOne);
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

void Core::NewTetromino(bool isPlayerOne)
{
	if (isPlayerOne) {
		playerOnePiece = { &blocksTexture, colors[nextColor],nextColor, Vector2i((gridWidth % 2 == 0 ? -blockSize / 2 : 0) + gridWidth *(currentParents[isPlayerOne ? 0 : 1] + 0.5f) * blockSize, 48), pieceType };
		playerOnePiece.SetParent(&gridGroup[currentParents[isPlayerOne ? 0 : 1]]);
	}
	else {
		playerTwoPiece = { &blocksTexture, colors[nextColor],nextColor, Vector2i((gridWidth % 2 == 0 ? -blockSize / 2 : 0) + gridWidth *(currentParents[isPlayerOne ? 0 : 1] + 0.5f) * blockSize, 48), pieceType };
		playerTwoPiece.SetParent(&gridGroup[currentParents[isPlayerOne ? 0 : 1]]);
	}
	pieceType = (Tetromino::TetrominoType)(rand() % 7);
	nextColor = rand() % gridGroup.size();
	nextPiece = { &blocksTexture, colors[nextColor],nextColor, Vector2i(blockSize*gridWidth * (gridGroup.size() + 0.5f),190), pieceType };
}

void Core::UpdateParent(int prevParent, bool isPlayerOne)//Try to change parent if position is not valid return to prevParent
{
	auto piece = isPlayerOne ? &playerOnePiece : &playerTwoPiece;
	auto currentPosition = piece->GetPosition();
	auto pieceCoords = gridGroup[prevParent].GetCoords(piece->GetPosition());
	auto vectorVal = gridGroup[currentParents[isPlayerOne ? 0 : 1]].GetVector(pieceCoords[0], pieceCoords[1]);
	vectorVal.x += blockSize / 2;
	vectorVal.y += blockSize / 2;
	piece->ClearParentInfo();
	piece->SetParent(&gridGroup[currentParents[isPlayerOne ? 0 : 1]]);
	piece->SetPosition(vectorVal);
	if (!piece->IsValidGridPos() || !piece->UpdateParentInfo(isPlayerOne ? 1 : 2))
	{
		currentParents[isPlayerOne ? 0 : 1] = prevParent;
		piece->SetParent(&gridGroup[currentParents[isPlayerOne ? 0 : 1]]);
		piece->SetPosition(currentPosition);
	}

}
