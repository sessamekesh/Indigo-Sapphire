#pragma once

#include <view/GenericVertex.h>
#include <memory>
#include <map>

namespace view
{
	template <typename ShaderType>
	class Entity
	{
		static_assert(std::is_base_of<ShaderType, view::Shader>::value, "ShaderType must be a base class of view::Shader");
	public:

		virtual bool prepare(std::shared_ptr<std::map<std::string, view::GenericMesh>> rawEntity, std::shared_ptr<ShaderType> shader, util::PipelineState& pso) = 0;
		virtual bool release() = 0;
	};
}