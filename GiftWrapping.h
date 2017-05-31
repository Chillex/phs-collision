#pragma once

#include <glm/glm.hpp>
#include <vector>

enum Orientation
{
	collinear,
	clockwise,
	counter_clockwise
};

template <typename V>
Orientation point_orientation(V p, V q, V r)
{
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return collinear;

	return (val > 0) ? clockwise : counter_clockwise;
}

namespace sf {
	class RenderWindow;
}

class GiftWrapping
{
public:
	explicit GiftWrapping(std::vector<glm::vec2>& points);
	
	bool ProcessNextStep(void);
	void OptimizedCalc(void);

	std::vector<glm::vec2> GetHull(void);
private:

	void SetupInitialState(void);

	std::vector<glm::vec2>& m_points;
	std::vector<glm::vec2> m_hull;

	// Some algorithm states - needed to step through the algo
	glm::vec2 m_leftmostPoint; // Start point, is definitely on hull
	int   m_leftmostIndex;
	glm::vec2 m_currentBestGuess; // Point that is currently the best guess for hull membership
	int m_currentBestGuessIndex;
	glm::vec2 m_currentCheck; // If that one is better than best guess, take it !
	int m_currentCheckIndex;
};
