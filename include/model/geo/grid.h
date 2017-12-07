#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace model
{
	namespace geo
	{

		class Grid
		{
		public:
			Grid(float subdivisionWidth, float subdivisionHeight, float gridWidth, float gridHeight, glm::vec4 gridlineColor, float gridlineWidth, float gridlineDepth, glm::vec3 originPoint, glm::quat orientation, bool isSolid, glm::vec4 backColor);

			glm::vec3 pos();
			glm::quat orientation();

		private:
			float _subWidth;
			float _subHeight;
			float _gridWidth;
			float _gridHeight;
			glm::vec4 _lineColor;
			glm::quat _orientation;
			float _lineWidth;
			float _lineDepth;
			bool _isSolid;
			glm::vec3 _originPoint;
			glm::vec4 _backColor;
		};

	}
}