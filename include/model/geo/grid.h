#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <model/geo/Line.h>
#include <vector>

namespace model
{
	namespace geo
	{
		class Grid
		{
		public:
			Grid(
				float subdivisionWidth,
				float subdivisionHeight,
				float gridWidth,
				float gridHeight,
				float gridlineRadius,
				glm::vec3 originPoint,
				glm::quat orientation,
				bool isSolid,
				glm::vec4 backColor
			);

			glm::vec3 pos();
			glm::quat orientation();

			std::vector<model::geo::Line> getLines() const;

		private:
			float _subWidth;
			float _subHeight;
			float _gridWidth;
			float _gridHeight;
			glm::quat _orientation;
			float _lineRadius;
			bool _isSolid;
			glm::vec3 _originPoint;
			glm::vec4 _backColor;
		};

	}
}