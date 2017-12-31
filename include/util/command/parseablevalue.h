#pragma once

#include <string>
#include <memory>

namespace util
{
	namespace command
	{
		struct ParseableValue
		{
			std::string ParserUUID;
			std::shared_ptr<void> ref;
		};
	}
}