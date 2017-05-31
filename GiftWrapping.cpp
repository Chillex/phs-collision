#include "GiftWrapping.h"

#include <algorithm>

GiftWrapping::GiftWrapping(std::vector<glm::vec2>& points)
	: m_points(points)
{
	SetupInitialState();
}

/**
 * Find leftmost point
 * Set first bestGuessPoint
 * Initialize currentCheckPoint to zero
 */
void GiftWrapping::SetupInitialState()
{
	std::vector<glm::vec2>::iterator min_it = std::min_element(m_points.begin(), m_points.end(), [](const glm::vec2& p, const glm::vec2& q) { return p.x < q.x; });
	m_leftmostIndex = std::distance(m_points.begin(), min_it);
	m_leftmostPoint = *min_it;
	m_hull.push_back(m_leftmostPoint);

	m_currentBestGuessIndex = (m_leftmostIndex + 1) % m_points.size();
	m_currentBestGuess = m_points[m_currentBestGuessIndex];

	m_currentCheckIndex = 0;
	m_currentCheck = m_points[m_currentCheckIndex];
}

/**
 * Process the next step in the algorithm
 * Can be executed as reaction to user input or in defined intervals
 * to better visualize the gift-wrapping algorithm
 */
bool GiftWrapping::ProcessNextStep()
{
	if (m_currentCheckIndex < m_points.size())
	{
		if (point_orientation(m_leftmostPoint, m_currentCheck, m_currentBestGuess) == counter_clockwise)
		{
			m_currentBestGuessIndex = m_currentCheckIndex;
			m_currentBestGuess = m_currentCheck;
		}

		m_currentCheck = m_points[m_currentCheckIndex];
		++m_currentCheckIndex;
	}
	else
	{
		m_hull.push_back(m_currentBestGuess);
		m_leftmostPoint = m_currentBestGuess;
		m_leftmostIndex = m_currentBestGuessIndex;

		if (m_currentBestGuess.x == m_hull[0].x && m_currentBestGuess.y == m_hull[0].y) return false;

		m_currentBestGuessIndex = (m_leftmostIndex + 1) % m_points.size();
		m_currentBestGuess = m_points[m_currentBestGuessIndex];

		// Reset the check variables that mimic the loop
		m_currentCheckIndex = 0;
		m_currentCheck = m_points[m_currentCheckIndex];
	}

	return true;
}

/**
 * Optimized version of the gift-wrapping algorithm to enable
 * performance measurements with a high resoultion chrono
 */
void GiftWrapping::OptimizedCalc()
{
	int p = m_leftmostIndex, q;

	do
	{
		q = (p + 1) % m_points.size();
		for (int i = 0; i < m_points.size(); ++i)
		{
			if (point_orientation(m_points[p], m_points[i], m_points[q]) == counter_clockwise)
				q = i;
		}

		m_hull.push_back(m_points[q]);
		p = q;

	} while (p != m_leftmostIndex);

}

std::vector<glm::vec2> GiftWrapping::GetHull(void)
{
	return m_hull;
}
