#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Triangle;

namespace sf
{
	class RenderWindow;
}

struct Side
{
	enum Enum {
		Left = 0,
		Right = 1,
		None = 2
	};
};

struct CollisionChecks
{
public:
	static bool AABB(const Triangle& triangle1, const Triangle& triangle2);
	static bool OOBB(const Triangle& triangle1, const Triangle& triangle2);
	static bool Minkowski(const Triangle& triangle1, const Triangle& triangle2);
	static bool Minkowski(const Triangle& triangle1, const Triangle& triangle2, sf::RenderWindow& window);

private:
	static bool OBBOverlap(const Triangle& triangle1, const Triangle& triangle2);
	static void SATTest(const glm::vec2& axis, const std::vector<glm::vec2>& points, float& min, float& max);
	static bool Overlaps(float min1, float max1, float min2, float max2);
	static bool IsBetweenOrdered(float val, float lowerBound, float upperBound);
	static bool PointInConvexShape(const glm::vec2& point, const std::vector<glm::vec2>& shape);
};
