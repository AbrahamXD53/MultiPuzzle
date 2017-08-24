#include <SFML/Graphics.hpp>
#include "Core.h"
#include <iostream>
#include <SFML/Window/Event.hpp>

using namespace std;
using namespace sf;

const int GridWidth = 12;
const int GridHeight = 22;
const int BlockSize = 32;


int main()
{
	RenderWindow window(sf::VideoMode(1280, 720), "Puzzle", Style::Close);
	window.setFramerateLimit(60);

	Core gameCore;
	if (gameCore.Init(&window, GridWidth, GridHeight, BlockSize) < 0)
		return -1;

	Clock clock;
	int timeTick = 1000;
	bool gameOver = false;

	int mouseGrid = 0;
	Vector2i mouseCoords;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::MouseMoved) {
				gameCore.CalculateMousePosition();
			}
			if (event.type == Event::MouseButtonPressed) {
				if (Mouse::isButtonPressed(Mouse::Left))
					gameCore.ProcessRemoval();
			}
			
			if (event.type == Event::KeyPressed) {

				if (Keyboard::isKeyPressed(Keyboard::Escape))
					window.close();
				if (Keyboard::isKeyPressed(Keyboard::R))
				{
					if (gameOver) {
						gameOver = false;
						int timeTick = 1000;
						if (gameCore.Init(&window, GridWidth, GridHeight, BlockSize) < 0)
							return -1;
					}
				}
				if (gameOver)
					break;

				if (Keyboard::isKeyPressed(Keyboard::Numpad0))
					gameCore.SwitchPlayer2();
				if (Keyboard::isKeyPressed(Keyboard::L))
					gameCore.FlipBoard();
				
				if (!gameCore.UpdateInput(event.key.code))
					gameOver = true;
			}
		}

		if (clock.getElapsedTime().asMilliseconds() >= timeTick && !gameOver)
		{
			clock.restart();
			if (timeTick > 500)
				timeTick -= 5;
			else
				if (timeTick > 300)
					timeTick--;
			if (!gameCore.UpdatePhisycs(true))
				gameOver=true;
			if (!gameCore.UpdatePhisycs(false))
				gameOver = true;
		}
		if (!gameOver)
			if (!gameCore.Update())
				gameOver = true;

		/*Clear n Draw*/
		window.clear();

		if (!gameOver)
			gameCore.Draw();
		else
			gameCore.ShowGameOver();

		/*Present*/
		window.display();
	}
	return 0;
}