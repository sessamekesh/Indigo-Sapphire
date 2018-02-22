#pragma once

/**
 * MenuItemValueBase
 *
 * MenuItems contain a label and a value - the value can be configured, and whatever is using the menu item
 *  can listen to changes, check the value, change the value, etc.
 * A MenuItemValue can pull many types of values, depending on the context of what is using it.
 * They do all have to provide their current value and expose get/set methods
 */

#include <functional>
#include <map>

namespace model
{
	namespace menu
	{
		template <typename Value>
		class MenuItemValueBase
		{
		public:
			MenuItemValueBase(const Value& initialValue);

			// Enforce that the value being set conforms to the attempted set value.
			//  If it does not, change and return a value that does conform.
			virtual Value enforceValueBounds(const Value& attemptedVal) const;

			// Set the value
			void setValue(const Value& attemptedVal);

			// Get the value
			Value getValue() const;

			// Setup a listener that fires whenever the value is set, and return a handle
			//  that can later be unlistened
			std::uint32_t listenForChanges(std::function<void(const Value& newval)> listener);

			// Unlisten to a previously listening listener
			void unlistenForChanges(std::uint32_t handle);

		protected:
			// When the value is read, optionally call this instead of fetching the current value
			virtual bool overrideGet(Value& o_useMeInstead);

		protected:
			Value currentValue_;
			std::map<std::uint32_t, std::function<void(const Value& newval)>> listeners_;
			std::uint32_t nextListenerIdx_;
		};

		///////////////////////////////////
		//
		// Definitions
		//
		///////////////////////////////////
		template<typename Value>
		MenuItemValueBase<Value>::MenuItemValueBase(const Value& initialValue)
			: currentValue_(initialValue)
			, listeners_({})
			, nextListenerIdx_(0u)
		{}

		template<typename Value>
		Value MenuItemValueBase<Value>::enforceValueBounds(const Value& attemptedVal) const
		{
			return attemptedVal;
		}

		template<typename Value>
		void MenuItemValueBase<Value>::setValue(const Value& attemptedVal)
		{
			Value use = enforceValueBounds(attemptedVal);
			if (currentValue_ != use)
			{
				currentValue_ = use;
				for (auto&& listener : listeners_)
				{
					listener.second(use);
				}
			}
		}

		template<typename Value>
		Value MenuItemValueBase<Value>::getValue() const
		{
			Value tr;
			if (overrideGet(tr))
			{
				if (currentValue_ != tr)
				{
					currentValue_ = tr;
					for (auto&& listener : listeners_)
					{
						listener.second(tr);
					}
				}
			}
			
			return currentValue_;
		}

		template<typename Value>
		std::uint32_t MenuItemValueBase<Value>::listenForChanges(std::function<void(const Value& newval)> listener)
		{
			auto idx = nextListenerIdx_++;
			listeners_[idx] = listener;
			return idx;
		}

		template<typename Value>
		void MenuItemValueBase<Value>::unlistenForChanges(std::uint32_t handle)
		{
			listeners_.erase(handle);
		}

		template<typename Value>
		bool MenuItemValueBase<Value>::overrideGet(Value& ov)
		{
			// Default behavior is not to override getting the value
			return false;
		}
	}
}