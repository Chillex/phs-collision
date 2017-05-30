#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include <SFML/Graphics.hpp>

class FPSCounter
{
public:
	FPSCounter(std::string fontFile);
	~FPSCounter();

	void Update(sf::Time deltaTime);
	void Draw(sf::RenderWindow& window) const;

private:
	int m_frameCount;
	sf::Time m_elapsedTime;

	sf::Text m_text;
	sf::Font m_font;
};

#endif
