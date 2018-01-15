#pragma once

//
// The event base forms something that must be started by some condition,
//  and then proceeds. That's it. Easy enough.
//

namespace model
{
	namespace evt
	{
		class EventBase
		{
		public:
			EventBase();
			~EventBase() = default;
			EventBase(const EventBase&) = default;

			void start();
			void pause();
			void resume();
			void finish();

			void update(float dt);

			bool isRunning() const;
			bool isFinished() const;

		protected:
			virtual void updateInternal(float dt) = 0;

		private:
			bool isStarted_;
			bool isFinished_;
			bool isRunning_;
		};
	}
}