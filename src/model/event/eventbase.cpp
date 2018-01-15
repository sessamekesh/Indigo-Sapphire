#include <model/event/eventbase.h>

namespace model
{
	namespace evt
	{
		EventBase::EventBase()
			: isStarted_(false)
			, isFinished_(false)
			, isRunning_(false)
		{}

		void EventBase::start()
		{
			isStarted_ = true;
			isRunning_ = true;
			isFinished_ = false;
		}

		void EventBase::pause()
		{
			isRunning_ = false;
		}

		void EventBase::resume()
		{
			isRunning_ = true;
		}

		void EventBase::finish()
		{
			isRunning_ = false;
			isFinished_ = true;
		}

		void EventBase::update(float dt)
		{
			if (isStarted_ && isRunning_)
			{
				updateInternal(dt);
			}
		}

		bool EventBase::isRunning() const
		{
			return isRunning_;
		}

		bool EventBase::isFinished() const
		{
			return isFinished_;
		}
	}
}