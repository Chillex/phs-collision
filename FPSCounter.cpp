#include "FPSCounter.h"
#include <sstream>

FPSCounter::FPSCounter(std::string fontFile)
	: m_frameCount(0)
	, m_elapsedTime(sf::seconds(0.0f))
{
	m_font.loadFromFile(fontFile);

	m_text.setFont(m_font);
	m_text.setFillColor(sf::Color(255, 64, 64, 255));
	m_text.setCharacterSize(24);
	m_text.setPosition(10, 10);
	m_text.setString("0 FPS");
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::Update(sf::Time deltaTime)
{
	++m_frameCount;
	m_elapsedTime += deltaTime;

	if(m_elapsedTime.asSeconds() >= 0.25f)
	{
		std::stringstream fps;
		fps << m_frameCount * 4 << " FPS";
		m_text.setString(fps.str());

		m_frameCount = 0;
		m_elapsedTime = sf::seconds(0.0f);
	}
}

void FPSCounter::Draw(sf::RenderWindow& window) const
{
	window.draw(m_text);
}
