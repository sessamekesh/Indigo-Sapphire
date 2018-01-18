#pragma once

#include <glad/glad.h>
#include <util/Logger.h>
#include <util/pipelinestate.h>
#include <vector>
#include <view/GenericVertex.h>

namespace view
{
	class Shader
	{
	public:
		Shader(
			std::string logName,
			std::string vsFileName,
			std::string fsFileName
		);
		~Shader();
		Shader(const Shader&) = delete;

		bool initialize();
		bool activate();
		void setVertexAttribPointers(util::PipelineState& pso);

		virtual std::vector<std::uint8_t> getVertexBuffer(const std::vector<view::GenericVertex>& genericVertices) = 0;

	protected:
		virtual void setVertexAttribPointersInternal() = 0;
		virtual unsigned int getNumVertexAttribPointers() = 0;
		virtual bool getUniformLocations() = 0;

		GLuint program_;
		util::Logger log;

	private:
		bool isLoaded_;

		static const unsigned long MAX_INFO_LOG_LENGTH = 512l;
		std::string vsFname_, fsFname_;
	};
}