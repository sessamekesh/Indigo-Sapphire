#include <util/command/floatparser.h>
#include <sstream>

namespace util
{
	namespace command
	{
		const std::string FloatParser::uuid_ = std::string("float32");

		bool FloatParser::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() > 0)
			{
				return false;
			}

			std::istringstream iss(cmd.val);
			float v = 0.f;
			if ((iss >> v).fail())
			{
				return false;
			}
			else
			{
				*std::static_pointer_cast<float>(ptr) = v;
				return true;
			}
		}

		std::vector<PropertyCommand> FloatParser::serializeValue(std::shared_ptr<void> ptr)
		{
			float v = *std::static_pointer_cast<float>(ptr);
			std::stringstream ss("");
			ss << v;
			return { { {}, ss.str() } };
		}

		std::string FloatParser::getUUID()
		{
			return FloatParser::uuid_;
		}
	}
}