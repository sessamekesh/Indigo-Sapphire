#include <model/geo/grid.h>
#include <view/solidshader/solidshader.h>

namespace model
{
	namespace geo
	{
		Grid::Grid(float subdivisionWidth, float subdivisionHeight, float gridWidth, float gridHeight,
			float gridlineRadius, glm::vec3 originPoint, glm::quat orientation,
			bool isSolid, glm::vec4 backColor
		)
			: _subWidth(subdivisionWidth)
			, _subHeight(subdivisionHeight)
			, _gridWidth(gridWidth)
			, _gridHeight(gridHeight)
			, _orientation(orientation)
			, _lineRadius(gridlineRadius)
			, _isSolid(isSolid)
			, _originPoint(originPoint)
			, _backColor(backColor)
		{
		}

		glm::vec3 Grid::pos()
		{
			return _originPoint;
		}

		glm::quat Grid::orientation()
		{
			return _orientation;
		}

		std::vector<model::geo::Line> Grid::getLines() const
		{
			std::vector<model::geo::Line> tr;
			
			tr.reserve(_gridWidth / _subWidth + _gridHeight / _subHeight);

			for (auto startX = 0.f; startX <= _gridWidth; startX += _subWidth)
			{
				tr.push_back(model::geo::Line(
					glm::vec3(startX, 0.f, 0.f),
					glm::vec3(startX, _gridHeight, 0.f),
					_lineRadius
				));
			}

			for (auto startY = 0.f; startY <= _gridHeight; startY += _subHeight)
			{
				tr.push_back(model::geo::Line(
					glm::vec3(0.f, startY, 0.f),
					glm::vec3(_gridWidth, startY, 0.f),
					_lineRadius
				));
			}
			
			return tr;
		}
	}
}