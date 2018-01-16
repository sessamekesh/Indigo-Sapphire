#pragma once

#include <sim/lakescene/shaders/tree.h>
#include <vector>
#include <view/texture.h>
#include <memory>
#include <optional>

namespace sim
{
	namespace lake
	{
		struct TreeMesh
		{
			std::vector<TreeShader::Vertex> vertices;
			std::vector<std::uint32_t> indices;
		};

		std::optional<std::vector<TreeMesh>> loadFromFile(
			std::string fName,
			util::Logger& log
		);
	}
}