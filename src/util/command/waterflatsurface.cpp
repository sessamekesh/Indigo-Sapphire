#include <util/command/waterflatsurface.h>
#include <view/special/watersurface/flatsurface.h>

namespace util
{
	namespace command
	{
		const std::string WaterFlatSurface::uuid("WaterFlatSurface");

		WaterFlatSurface::WaterFlatSurface(
			const WithWorldTransformParser& wwp,
			const FloatParser& fp
		)
			: withWorldTransform_(wwp)
			, float_(fp)
		{}

		bool WaterFlatSurface::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() < 1)
			{
				return false;
			}

			auto waterFlatSurface = std::static_pointer_cast<view::special::watersurface::FlatSurface>(ptr);

			// First, attempt to modify the model
			if (withWorldTransform_.setValue(cmd, waterFlatSurface->model()))
			{
				return true;
			}

			// It's not the model, so try for the other properties
			auto nextCmd = cmd;
			nextCmd.subVals.erase(nextCmd.subVals.begin());
			if (cmd.subVals[0] == "tiling")
			{
				auto nvf = std::make_shared<float>();
				if (!float_.setValue(nextCmd, nvf))
				{
					return false;
				}
				waterFlatSurface->tilingMultiplier(*nvf);
			}
			else if (cmd.subVals[0] == "dudvScale")
			{
				auto nvf = std::make_shared<float>();
				if (!float_.setValue(nextCmd, nvf))
				{
					return false;
				}
				waterFlatSurface->dudvScale(*nvf);
			}
			else if (cmd.subVals[0] == "offsetVelocity")
			{
				auto nvf = std::make_shared<float>();
				if (!float_.setValue(nextCmd, nvf))
				{
					return false;
				}
				waterFlatSurface->dudvOffsetVelocity(*nvf);
			}
			else if (cmd.subVals[0] == "shineDamper")
			{
				auto nvf = std::make_shared<float>();
				if (!float_.setValue(nextCmd, nvf))
				{
					return false;
				}
				waterFlatSurface->shineDamper(*nvf);
			}
			else if (cmd.subVals[0] == "reflectivity")
			{
				auto nvf = std::make_shared<float>();
				if (!float_.setValue(nextCmd, nvf))
				{
					return false;
				}
				waterFlatSurface->reflectivity(*nvf);
			}
			else
			{
				return false;
			}

			return true;
		}

		std::vector<PropertyCommand> WaterFlatSurface::serializeValue(std::shared_ptr<void> ptr)
		{
			auto wft = std::static_pointer_cast<view::special::watersurface::FlatSurface>(ptr);

			std::vector<PropertyCommand> tr;

			for (auto wwtVal : withWorldTransform_.serializeValue(wft->model()))
			{
				tr.push_back(wwtVal);
			}

			for (auto tv : float_.serializeValue(std::make_shared<float>(wft->tilingMultiplier())))
			{
				tv.subVals.emplace(tv.subVals.begin(), "tiling");
				tr.push_back(tv);
			}

			for (auto tv : float_.serializeValue(std::make_shared<float>(wft->dudvScale())))
			{
				tv.subVals.emplace(tv.subVals.begin(), "dudvScale");
				tr.push_back(tv);
			}

			for (auto tv : float_.serializeValue(std::make_shared<float>(wft->dudvOffsetVelocity())))
			{
				tv.subVals.emplace(tv.subVals.begin(), "offsetVelocity");
				tr.push_back(tv);
			}

			for (auto tv : float_.serializeValue(std::make_shared<float>(wft->shineDamper())))
			{
				tv.subVals.emplace(tv.subVals.begin(), "shineDamper");
				tr.push_back(tv);
			}

			for (auto tv : float_.serializeValue(std::make_shared<float>(wft->reflectivity())))
			{
				tv.subVals.emplace(tv.subVals.begin(), "reflectivity");
				tr.push_back(tv);
			}

			return tr;
		}

		std::string WaterFlatSurface::getUUID()
		{
			return WaterFlatSurface::uuid;
		}
	}
}