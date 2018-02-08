#pragma once

#include <glad/glad.h>
#include <util/Logger.h>
#include <util/pipelinestate.h>
#include <vector>
#include <view/GenericVertex.h>
#include <optional>

namespace view
{
	class Shader
	{
	public:
		Shader(
			std::string logName,
			std::string vsFileName,
			std::string fsFileName,
			std::optional<std::string> gsFileName = {}
		);
		~Shader();
		Shader(const Shader&) = delete;

		bool initialize();
		bool activate();
		void setVertexAttribPointers(util::PipelineState& pso);

	protected:
		virtual void setVertexAttribPointersInternal() = 0;
		virtual unsigned int getNumVertexAttribPointers() = 0;
		virtual bool getUniformLocations() = 0;

		virtual bool initializeInternal() { return true; } // Can be overriden

		GLuint program_;
		util::Logger log;

	private:
		bool isLoaded_;

		static const unsigned long MAX_INFO_LOG_LENGTH = 512l;
		std::string vsFname_, fsFname_;
		std::optional<std::string> gsFname_;
	};
}