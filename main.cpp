#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

#include "FPSCounter.h"
#include "Triangle.h"

int main()
{
	sf::VideoMode vm(1280, 720);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(vm, "Collision Detector", sf::Style::Default, settings);
	window.setMouseCursorVisible(false);

	sf::View hudView(sf::FloatRect(0, 0, 1920, 1080));
	sf::View gameView(sf::FloatRect(0, 0, 1920, 1080));
	float zoom = 1.0f;
	bool middleMousePressed = false;
	sf::Vector2i lastMousePos;

	srand(time(NULL));

	FPSCounter fpsCounter("Assets/Font/digital_counter_7.ttf");
	sf::Color clearColor(38, 11, 1);

	Triangle movingTriangle = Triangle::GenerateRandom({ 100.0f, 100.0f }, { 0.0f, 0.0f });

	int staticTriangleCount = 300;
	std::vector<Triangle> staticTriangles;
	staticTriangles.reserve(staticTriangleCount);
	for (int i = 0; i < staticTriangleCount; ++i)
	{
		float x = rand() % (1920 * 2) - 960;
		float y = rand() % (1080 * 2) - 540;
		staticTriangles.emplace_back(Triangle::GenerateRandom({ 100.0f, 100.0f }, { x, y }));
	}

	sf::Clock deltaClock;
	sf::Time dt;
	while (window.isOpen())
	{
		dt = deltaClock.restart();
		window.setView(gameView);

		sf::Vector2i mousePosPixel = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosPixel);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			// key press events
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::R)
				{
					movingTriangle = Triangle::GenerateRandom({ 100.0f, 100.0f }, { 0.0f, 0.0f });
				}
			}

			// mouse scrool events
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.delta > 0.0f)
				{
					gameView.zoom(0.9f);
					zoom *= 0.9f;
				}
				else if (event.mouseWheelScroll.delta < 0.0f)
				{
					gameView.zoom(1.1f);
					zoom *= 1.1f;
				}
			}

			// mouse button events
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Middle)
					middleMousePressed = true;
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Middle)
					middleMousePressed = false;
			}
		}

		if (middleMousePressed)
		{
			sf::Vector2f mouseDelta(sf::Vector2f(lastMousePos.x - mousePosPixel.x, lastMousePos.y - mousePosPixel.y));
			gameView.move(zoom * mouseDelta);
		}
		lastMousePos = mousePosPixel;

		// updates
		fpsCounter.Update(dt);
		movingTriangle.position = { mousePosWorld.x, mousePosWorld.y };

		// test collision between static triangles
		for (size_t i = 0; i < staticTriangleCount; ++i)
		{
			staticTriangles[i].CalculateCollision(staticTriangles);
		}

		// test collision from the moving triangle
		movingTriangle.CalculateCollision(staticTriangles);

		window.clear(clearColor);
		
		// draws
		for (size_t i = 0; i < staticTriangleCount; ++i)
		{
			staticTriangles[i].Draw(window);
		}

		movingTriangle.Draw(window);

		window.setView(hudView);
		fpsCounter.Draw(window);

		window.display();
	}

	return 0;
}