#pragma once

#include <view/GenericVertex.h>
#include <memory>
#include <vector>
#include <map>
#include <util/Logger.h>

namespace view
{
	std::shared_ptr<std::map<std::string, view::GenericMesh>> loadFromScene(std::string fName, util::Logger log);
}