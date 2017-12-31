#include <util/command/vec3parser.h>
#include <glm/glm.hpp>
#include <sstream>
#include <string>

namespace util
{
	namespace command
	{
		const std::string Vec3Parser::uuid("vec3");

		Vec3Parser::Vec3Parser(const FloatParser& fp)
			: float_(fp)
		{}

		bool Vec3Parser::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() > 1)
			{
				return false;
			}

			auto vec3Ptr = std::reinterpret_pointer_cast<glm::vec3>(ptr);
			if (cmd.subVals.size() == 1)
			{
				switch (cmd.subVals[0][0])
				{
				case 'x':
				case 'X':
				{
					auto fp = std::shared_ptr<float>(&(vec3Ptr->x), [](float*) {});
					return float_.setValue({ {}, cmd.val }, fp);
				}
				case 'y':
				case 'Y':
				{
					auto fp = std::shared_ptr<float>(&(vec3Ptr->y), [](float*) {});
					return float_.setValue({ {}, cmd.val }, fp);
				}
				case 'z':
				case 'Z':
				{
					auto fp = std::shared_ptr<float>(&(vec3Ptr->z), [](float*) {});
					return float_.setValue({ {}, cmd.val }, fp);
				}
				default:
					return false;
				}
			}

			std::stringstream ss(cmd.val);
			std::string item;
			std::vector<std::string> tokens;
			while (std::getline(ss, item, ','))
			{
				tokens.push_back(item);
			}
			if (tokens.size() != 3)
			{
				return false;
			}

			float x, y, z;
			auto xp = std::shared_ptr<float>(&x, [](float*) {});
			auto yp = std::shared_ptr<float>(&y, [](float*) {});
			auto zp = std::shared_ptr<float>(&z, [](float*) {});

			bool result = float_.setValue({ {}, tokens[0] }, xp)
				&& float_.setValue({ {}, tokens[1] }, yp)
				&& float_.setValue({ {}, tokens[2] }, zp);

			if (result)
			{
				vec3Ptr->x = x;
				vec3Ptr->y = y;
				vec3Ptr->z = z;
				return true;
			}
			else
			{
				return false;
			}
		}

		std::vector<PropertyCommand> Vec3Parser::serializeValue(std::shared_ptr<void> ptr)
		{
			auto vp = std::reinterpret_pointer_cast<glm::vec3>(ptr);
			auto xp = std::shared_ptr<float>(&vp->x, [](float*) {});
			auto yp = std::shared_ptr<float>(&vp->y, [](float*) {});
			auto zp = std::shared_ptr<float>(&vp->z, [](float*) {});
			auto xval = float_.serializeValue(xp);
			auto yval = float_.serializeValue(yp);
			auto zval = float_.serializeValue(zp);

			std::stringstream ss("");
			ss << xval[0].val << "," << yval[0].val << "," << zval[0].val;
			return { { {}, ss.str()} };
		}

		std::string Vec3Parser::getUUID()
		{
			return Vec3Parser::uuid;
		}
	}
}