#pragma once

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

struct CollisionStatus
{
	enum Enum
	{
		None = 0,
		Circle = 1,
		AABB = 2,
		OBB = 3,
		Minkowski = 4
	};
};

struct Triangle {
	// world pos of the triangle
	glm::vec2 position;
	
	// points of the triangle, relativ to position
	glm::vec2 relativeP0;
	glm::vec2 relativeP1;
	glm::vec2 relativeP2;

	// circumcenter, relativ to position
	glm::vec2 bCircleCenter;
	float bCircleRadius;

	// width and height of AABB
	glm::vec2 aabbDimensions;

	// OBB
	glm::vec2 obbP0;
	glm::vec2 obbP1;
	glm::vec2 obbP2;
	glm::vec2 obbP3;

	CollisionStatus::Enum collisionStatus;

	friend bool operator==(const Triangle& lhs, const Triangle& rhs)
	{
		if (lhs.position != rhs.position)
			return false;

		if (lhs.relativeP0 != rhs.relativeP0 || lhs.relativeP1 != rhs.relativeP1 || lhs.relativeP2 != rhs.relativeP2)
			return false;

		return true;
	}

	friend bool operator!=(const Triangle& lhs, const Triangle& rhs)
	{
		return !(lhs == rhs);
	}

	static Triangle GenerateRandom(glm::vec2 size, glm::vec2 position)
	{
		Triangle triangle;

		float min = 5.0f;

		float randX = rand() % static_cast<int>(size.x * 0.5f - min + 1) + min;
		triangle.relativeP0 = { -randX, 0.0f };

		float randY = rand() % static_cast<int>(size.y * 0.5f - min + 1) + min;
		triangle.relativeP1 = { 0.0f, randY };

		float randX2 = rand() % static_cast<int>(size.x * 0.5f - min + 1) + min;
		float randY2 = rand() % static_cast<int>(size.y * 0.5f - min + 1) + min;
		triangle.relativeP2 = { randX, -randY };

		triangle.position = position;

		triangle.CalculateCircumcenter();
		triangle.CalculateAABB();
		triangle.CalculateOBB();

		triangle.collisionStatus = CollisionStatus::None;

		return triangle;
	}

	void CalculateCircumcenter(void)
	{
		bCircleCenter = (relativeP0 + relativeP1 + relativeP2) / 3.0f;
		bCircleRadius = std::max(glm::distance(bCircleCenter, relativeP0), std::max(glm::distance(bCircleCenter, relativeP1), glm::distance(bCircleCenter, relativeP2)));
	}

	void CalculateAABB(void)
	{
		float minX = std::min(relativeP0.x, std::min(relativeP1.x, relativeP2.x));
		float maxX = std::max(relativeP0.x, std::max(relativeP1.x, relativeP2.x));

		float minY = std::min(relativeP0.y, std::min(relativeP1.y, relativeP2.y));
		float maxY = std::max(relativeP0.y, std::max(relativeP1.y, relativeP2.y));

		aabbDimensions = { maxX - minX, maxY - minY };
	}

	void CalculateOBB(void)
	{
		glm::vec2 A = relativeP0 - relativeP1;
		float a = glm::length(A);
		glm::vec2 B = relativeP1 - relativeP2;
		float b = glm::length(B);
		glm::vec2 C = relativeP2 - relativeP0;
		float c = glm::length(C);

		glm::vec2 otherPoint;
		glm::vec2 line;
		if (a > b && a > c)
		{
			obbP0 = relativeP0;
			obbP1 = relativeP1;

			otherPoint = relativeP2;
			line = A;
		}
		else if (b > c)
		{
			obbP0 = relativeP1;
			obbP1 = relativeP2;

			otherPoint = relativeP0;
			line = B;
		}
		else
		{
			obbP0 = relativeP2;
			obbP1 = relativeP0;

			otherPoint = relativeP1;
			line = C;
		}

		float distance = DistanceFromLine(obbP0, obbP1, otherPoint);
		
		glm::vec2 normal(-line.y, line.x);
		normal = glm::normalize(normal);

		obbP2 = obbP1 + normal * distance;
		obbP3 = obbP0 + normal * distance;
	}

	float DistanceFromLine(glm::vec2 P, glm::vec2 Q, glm::vec2 X)
	{
		return std::abs((Q.y - P.y)*X.x - (Q.x - P.x)*X.y + Q.x*P.y - Q.y*P.x) / std::sqrt(std::pow((Q.y - P.y), 2) + std::pow((Q.x - P.x), 2));
	}

	void CalculateCollision(std::vector<Triangle>& otherTriangles)
	{
		collisionStatus = CollisionStatus::None;

		for (size_t i = 0; i < otherTriangles.size(); ++i)
		{
			Triangle& other = otherTriangles[i];
			if (*this == other)
				continue;
			
			// Circle - Circle Collision
			float distance = glm::distance(position + bCircleCenter, other.position + other.bCircleCenter);
			bool circleCollision = distance <= (bCircleRadius + other.bCircleRadius);

			if (!circleCollision)
			{
				continue;
			}

			// AABB Collision
			if (!CheckAABBCollision(other))
			{
				if (collisionStatus < CollisionStatus::Circle) collisionStatus = CollisionStatus::Circle;
				if (other.collisionStatus < CollisionStatus::Circle) other.collisionStatus = CollisionStatus::Circle;
				continue;
			}

			// OBB Collision
			if (!CheckOBBCollision(other))
			{
				if (collisionStatus < CollisionStatus::AABB) collisionStatus = CollisionStatus::AABB;
				if (other.collisionStatus < CollisionStatus::AABB) other.collisionStatus = CollisionStatus::AABB;
				continue;
			}

			collisionStatus = CollisionStatus::OBB;
			other.collisionStatus = CollisionStatus::OBB;

			//// Minkowski
			//if (!CheckMinkowski(other))
			//{
			//	if (collisionStatus < CollisionStatus::OBB) collisionStatus = CollisionStatus::OBB;
			//	if (other.collisionStatus < CollisionStatus::OBB) other.collisionStatus = CollisionStatus::OBB;
			//	continue;
			//}

			//collisionStatus = CollisionStatus::Minkowski;
			//other.collisionStatus = CollisionStatus::Minkowski;
		}
	}

	bool CheckAABBCollision(const Triangle& other) const
	{
		float x = position.x + bCircleCenter.x - aabbDimensions.x * 0.5f;
		float y = position.y + bCircleCenter.y - aabbDimensions.y * 0.5f;

		float otherX = other.position.x + other.bCircleCenter.x - other.aabbDimensions.x * 0.5f;
		float otherY = other.position.y + other.bCircleCenter.y - other.aabbDimensions.y * 0.5f;

		if (x + aabbDimensions.x < otherX || x > otherX + other.aabbDimensions.x)
			return false;

		if (y + aabbDimensions.y < otherY || y > otherY + other.aabbDimensions.y)
			return false;

		return true;
	}

	bool CheckOBBCollision(const Triangle& other) const
	{
		return OBBOverlap(other) && other.OBBOverlap(*this);
	}

	bool OBBOverlap(const Triangle& other) const
	{
		glm::vec2 axis[2] = {
			glm::normalize((position + obbP1) - (position + obbP0)),
			glm::normalize((position + obbP3) - (position + obbP0))
		};

		for (int a = 0; a < 2; ++a)
		{
			float tri1Min, tri1Max, tri2Min, tri2Max;

			SATTest(axis[a], { position + obbP0, position + obbP1, position + obbP2, position + obbP3 }, tri1Min, tri1Max);
			SATTest(axis[a], { other.position + other.obbP0, other.position + other.obbP1, other.position + other.obbP2, other.position + other.obbP3 }, tri2Min, tri2Max);

			if (!Overlaps(tri1Min, tri1Max, tri2Min, tri2Max))
			{
				return false;
			}
		}

		return true;
	}

	void SATTest(const glm::vec2& axis, const std::vector<glm::vec2>& points, float& min, float& max) const
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

	bool Overlaps(float min1, float max1, float min2, float max2) const
	{
		return IsBetweenOrdered(min2, min1, max1) || IsBetweenOrdered(min1, min2, max2);
	}

	bool IsBetweenOrdered(float val, float lowerBound, float upperBound) const
	{
		return lowerBound <= val && val <= upperBound;
	}

	bool CheckMinkowski(const Triangle& other)
	{
		return false;
	}
};

void Draw(const Triangle& triangle, sf::RenderWindow& window)
{
	sf::VertexArray vertices(sf::Triangles, 3);

	vertices[0].position = { triangle.position.x + triangle.relativeP0.x, triangle.position.y + triangle.relativeP0.y };
	vertices[1].position = { triangle.position.x + triangle.relativeP1.x, triangle.position.y + triangle.relativeP1.y };
	vertices[2].position = { triangle.position.x + triangle.relativeP2.x, triangle.position.y + triangle.relativeP2.y };

	sf::Color triangleColor(252, 243, 208);

	if (triangle.collisionStatus >= CollisionStatus::Minkowski)
	{
		// draw Minkowski
		triangleColor = sf::Color::Red;
	}

	vertices[0].color = triangleColor;
	vertices[1].color = triangleColor;
	vertices[2].color = triangleColor;

	window.draw(vertices);

	if (triangle.collisionStatus >= CollisionStatus::Circle)
	{
		// draw bounding circle
		sf::CircleShape bCircle(triangle.bCircleRadius);

		bCircle.setFillColor(sf::Color::Transparent);
		bCircle.setOutlineColor(sf::Color(242, 170, 107));
		bCircle.setOutlineThickness(-1.0f);
		
		bCircle.setOrigin(triangle.bCircleRadius, triangle.bCircleRadius);
		bCircle.setPosition(triangle.position.x + triangle.bCircleCenter.x, triangle.position.y + triangle.bCircleCenter.y);

		window.draw(bCircle);
	}

	if (triangle.collisionStatus >= CollisionStatus::AABB)
	{
		// draw AABB
		sf::RectangleShape aabb({ triangle.aabbDimensions.x, triangle.aabbDimensions.y });

		aabb.setFillColor(sf::Color::Transparent);
		aabb.setOutlineColor(sf::Color(242, 92, 5));
		aabb.setOutlineThickness(-1.0f);

		aabb.setOrigin(triangle.aabbDimensions.x * 0.5f, triangle.aabbDimensions.y * 0.5f);
		aabb.setPosition(triangle.position.x + triangle.bCircleCenter.x, triangle.position.y + triangle.bCircleCenter.y);

		window.draw(aabb);
	}

	if (triangle.collisionStatus >= CollisionStatus::OBB)
	{
		// draw OBB
		sf::ConvexShape oob;
		oob.setPointCount(4);

		oob.setPoint(0, { triangle.position.x + triangle.obbP0.x, triangle.position.y + triangle.obbP0.y });
		oob.setPoint(1, { triangle.position.x + triangle.obbP3.x, triangle.position.y + triangle.obbP3.y });
		oob.setPoint(2, { triangle.position.x + triangle.obbP2.x, triangle.position.y + triangle.obbP2.y });
		oob.setPoint(3, { triangle.position.x + triangle.obbP1.x, triangle.position.y + triangle.obbP1.y });

		oob.setFillColor(sf::Color::Transparent);
		oob.setOutlineColor(sf::Color(242, 68, 5));
		oob.setOutlineThickness(-1.0f);

		window.draw(oob);
	}
}