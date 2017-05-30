#pragma once

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

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
		else if (b > c && b > a)
		{
			obbP0 = relativeP1;
			obbP1 = relativeP2;

			otherPoint = relativeP0;
			line = B;
		}
		else if (c > a && c > b)
		{
			obbP0 = relativeP2;
			obbP1 = relativeP0;

			otherPoint = relativeP1;
			line = C;
		}
		else
		{
			printf("no side is the longest for obb...");
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
};

void Draw(const Triangle& triangle, sf::RenderWindow& window, const CollisionStatus::Enum& collisionStatus)
{
	sf::VertexArray vertices(sf::Triangles, 3);

	vertices[0].position = { triangle.position.x + triangle.relativeP0.x, triangle.position.y + triangle.relativeP0.y };
	vertices[1].position = { triangle.position.x + triangle.relativeP1.x, triangle.position.y + triangle.relativeP1.y };
	vertices[2].position = { triangle.position.x + triangle.relativeP2.x, triangle.position.y + triangle.relativeP2.y };

	sf::Color triangleColor(252, 243, 208);

	if (collisionStatus >= CollisionStatus::Circle)
	{
		// draw bounding circle
		sf::CircleShape bCircle(triangle.bCircleRadius);

		bCircle.setFillColor(sf::Color::Transparent);
		bCircle.setOutlineColor(sf::Color(242, 170, 107));
		bCircle.setOutlineThickness(1.0f);
		
		bCircle.setOrigin(triangle.bCircleRadius, triangle.bCircleRadius);
		bCircle.setPosition(triangle.position.x + triangle.bCircleCenter.x, triangle.position.y + triangle.bCircleCenter.y);

		window.draw(bCircle);
	}

	if (collisionStatus >= CollisionStatus::AABB)
	{
		// draw AABB
		sf::RectangleShape aabb({ triangle.aabbDimensions.x, triangle.aabbDimensions.y });

		aabb.setFillColor(sf::Color::Transparent);
		aabb.setOutlineColor(sf::Color(242, 92, 5));
		aabb.setOutlineThickness(1.0f);

		aabb.setOrigin(triangle.aabbDimensions.x * 0.5f, triangle.aabbDimensions.y * 0.5f);
		aabb.setPosition(triangle.position.x + triangle.bCircleCenter.x, triangle.position.y + triangle.bCircleCenter.y);

		window.draw(aabb);
	}

	if (collisionStatus >= CollisionStatus::OBB)
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
		oob.setOutlineThickness(1.0f);

		window.draw(oob);
	}

	if (collisionStatus >= CollisionStatus::Minkowski)
	{
		// draw Minkowski
		triangleColor = sf::Color::Red;
	}

	vertices[0].color = triangleColor;
	vertices[1].color = triangleColor;
	vertices[2].color = triangleColor;

	window.draw(vertices);
}