#include <model/geo/grid.h>
#include <view/solidshader/solidshader.h>

namespace model
{
	namespace geo
	{
		Grid::Grid(float subdivisionWidth, float subdivisionHeight, float gridWidth, float gridHeight,
			glm::vec4 gridlineColor, float gridlineWidth, float gridlineDepth, glm::vec3 originPoint, glm::quat orientation,
			bool isSolid, glm::vec4 backColor
		)
			: _subWidth(subdivisionWidth)
			, _subHeight(subdivisionHeight)
			, _gridWidth(gridWidth)
			, _gridHeight(gridHeight)
			, _lineColor(gridlineColor)
			, _orientation(orientation)
			, _lineWidth(gridlineWidth)
			, _lineDepth(gridlineDepth)
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
	}
}