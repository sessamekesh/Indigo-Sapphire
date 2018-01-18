#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace view
{
	struct GenericVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
	};

	struct GenericMesh
	{
		std::vector<view::GenericVertex> vertices;
		std::vector<std::uint32_t> indices;
	};
}