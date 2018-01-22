#pragma once

#include <model/proctree/proctree.h>
#include <memory>
#include <view/GenericVertex.h>
#include <optional>
#include <util/Logger.h>

namespace view
{
	namespace raw
	{
		bool getProcTreeMesh(
			std::shared_ptr<Proctree::Tree> t,
			util::Logger& log,
			view::GenericMesh& o_Tree,
			view::GenericMesh& o_Twigs
		);
	}
}