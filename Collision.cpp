#include "Collision.h"

#include "Triangle.h"
#include "GiftWrapping.h"

bool CollisionChecks::AABB(const Triangle& triangle1, const Triangle& triangle2)
{
	float x = triangle1.position.x + triangle1.bCircleCenter.x - triangle1.aabbDimensions.x * 0.5f;
	float y = triangle1.position.y + triangle1.bCircleCenter.y - triangle1.aabbDimensions.y * 0.5f;

	float otherX = triangle2.position.x + triangle2.bCircleCenter.x - triangle2.aabbDimensions.x * 0.5f;
	float otherY = triangle2.position.y + triangle2.bCircleCenter.y - triangle2.aabbDimensions.y * 0.5f;

	if (x + triangle1.aabbDimensions.x < otherX || x > otherX + triangle2.aabbDimensions.x)
		return false;

	if (y + triangle1.aabbDimensions.y < otherY || y > otherY + triangle2.aabbDimensions.y)
		return false;

	return true;
}

bool CollisionChecks::OOBB(const Triangle& triangle1, const Triangle& triangle2)
{
	return OBBOverlap(triangle1, triangle2) && OBBOverlap(triangle2, triangle1);
}

bool CollisionChecks::Minkowski(const Triangle& triangle1, const Triangle& triangle2)
{
	// create minkowski points by adding the negated triangle2 to each point of triangle1
	std::vector<glm::vec2> minkowskiPoints;
	minkowskiPoints.reserve(9);

	glm::vec2 t1p0 = { triangle1.position + triangle1.relativeP0 };
	glm::vec2 t1p1 = { triangle1.position + triangle1.relativeP1 };
	glm::vec2 t1p2 = { triangle1.position + triangle1.relativeP2 };

	glm::vec2 t2p0 = { triangle2.position + triangle2.relativeP0 };
	glm::vec2 t2p1 = { triangle2.position + triangle2.relativeP1 };
	glm::vec2 t2p2 = { triangle2.position + triangle2.relativeP2 };

	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p0.x, t1p0.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p1.x, t1p0.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p2.x, t1p0.y - t2p2.y));

	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p0.x, t1p1.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p1.x, t1p1.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p2.x, t1p1.y - t2p2.y));

	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p0.x, t1p2.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p1.x, t1p2.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p2.x, t1p2.y - t2p2.y));

	// create convex hull
	GiftWrapping convexHull(minkowskiPoints);
	convexHull.OptimizedCalc();
	std::vector<glm::vec2> minkowskiShape = convexHull.GetHull();

	// check if origin is inside minkowski shape
	return PointInConvexShape(glm::vec2(0.0f, 0.0f), minkowskiShape);
}

bool CollisionChecks::Minkowski(const Triangle & triangle1, const Triangle & triangle2, sf::RenderWindow & window)
{
	// create minkowski points by adding the negated triangle2 to each point of triangle1
	std::vector<glm::vec2> minkowskiPoints;
	minkowskiPoints.reserve(9);

	glm::vec2 t1p0 = { triangle1.position + triangle1.relativeP0 };
	glm::vec2 t1p1 = { triangle1.position + triangle1.relativeP1 };
	glm::vec2 t1p2 = { triangle1.position + triangle1.relativeP2 };

	glm::vec2 t2p0 = { triangle2.position + triangle2.relativeP0 };
	glm::vec2 t2p1 = { triangle2.position + triangle2.relativeP1 };
	glm::vec2 t2p2 = { triangle2.position + triangle2.relativeP2 };

	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p0.x, t1p0.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p1.x, t1p0.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p0.x - t2p2.x, t1p0.y - t2p2.y));

	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p0.x, t1p1.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p1.x, t1p1.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p1.x - t2p2.x, t1p1.y - t2p2.y));

	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p0.x, t1p2.y - t2p0.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p1.x, t1p2.y - t2p1.y));
	minkowskiPoints.emplace_back(glm::vec2(t1p2.x - t2p2.x, t1p2.y - t2p2.y));

	// create convex hull
	GiftWrapping convexHull(minkowskiPoints);
	convexHull.OptimizedCalc();
	std::vector<glm::vec2> minkowskiShape = convexHull.GetHull();

	// draw OBB
	sf::ConvexShape hull;
	hull.setPointCount(minkowskiShape.size());

	for (size_t i = 0; i < minkowskiShape.size(); ++i)
	{
		hull.setPoint(i, { minkowskiShape[i].x, minkowskiShape[i].y });
	}

	hull.setFillColor(sf::Color::Transparent);
	hull.setOutlineColor(sf::Color::Green);
	hull.setOutlineThickness(1.0f);

	window.draw(hull);

	// check if origin is inside minkowski shape
	return PointInConvexShape(glm::vec2(0.0f, 0.0f), minkowskiShape);
}

bool CollisionChecks::OBBOverlap(const Triangle& triangle1, const Triangle& triangle2)
{
	glm::vec2 axis[2] = {
		glm::normalize((triangle1.position + triangle1.obbP1) - (triangle1.position + triangle1.obbP0)),
		glm::normalize((triangle1.position + triangle1.obbP3) - (triangle1.position + triangle1.obbP0))
	};

	for (int a = 0; a < 2; ++a)
	{
		float tri1Min, tri1Max, tri2Min, tri2Max;

		SATTest(axis[a], { triangle1.position + triangle1.obbP0, triangle1.position + triangle1.obbP1, triangle1.position + triangle1.obbP2, triangle1.position + triangle1.obbP3 }, tri1Min, tri1Max);
		SATTest(axis[a], { triangle2.position + triangle2.obbP0, triangle2.position + triangle2.obbP1, triangle2.position + triangle2.obbP2, triangle2.position + triangle2.obbP3 }, tri2Min, tri2Max);

		if (!Overlaps(tri1Min, tri1Max, tri2Min, tri2Max))
		{
			return false;
		}
	}

	return true;
}

void CollisionChecks::SATTest(const glm::vec2& axis, const std::vector<glm::vec2>& points, float& min, float& max)
{
	min = 99999999;
	max = -99999999;

	for (size_t i = 0; i < points.size(); ++i)
	{
		float dotValue = glm::dot(axis, points[i]);

		if (dotValue < min)
			min = dotValue;

		if (dotValue > max)
			max = dotValue;
	}
}

bool CollisionChecks::Overlaps(float min1, float max1, float min2, float max2)
{
	return IsBetweenOrdered(min2, min1, max1) || IsBetweenOrdered(min1, min2, max2);
}

bool CollisionChecks::IsBetweenOrdered(float val, float lowerBound, float upperBound)
{
	return lowerBound <= val && val <= upperBound;
}

bool CollisionChecks::PointInConvexShape(const glm::vec2& point, const std::vector<glm::vec2>& shape)
{
	Side::Enum previousSide = Side::None;

	for (size_t i = 0; i < shape.size(); ++i)
	{
		glm::vec2 a = shape[i];
		glm::vec2 b = shape[(i + 1) % (shape.size() - 1)];

		float cross = (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x);

		Side::Enum currentSide;
		if (cross < 0.0f)
			currentSide = Side::Right;
		else if (cross > 0.0f)
			currentSide = Side::Left;
		else
			currentSide = Side::None;

		if (currentSide == Side::None)
		{
			// outside or over an edge
			return false;
		}
		else if (previousSide == Side::None)
		{
			// first segment
			previousSide = currentSide;
		}
		else if (previousSide != currentSide)
		{
			// point not always on same side
			return false;
		}
	}

	return true;
}
