#pragma once

#include <view/solidshader/solidshader.h>
#include <optional>
#include <vector>
#include <string>
#include <util/Logger.h>
#include <glm/vec4.hpp>
#include <map>

namespace view
{
	namespace solidshader
	{
		namespace assets
		{
			// TODO SESS: Implement
			std::optional<std::map<std::string, BasicGeometry>> importFromFile(
				const std::string& file,
				const glm::vec4& defaultColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.f),
				std::optional<util::Logger> logger = std::nullopt,
				const std::optional<std::vector<std::string>>& modelNamesOpt = {}
			);
		}
	}
}