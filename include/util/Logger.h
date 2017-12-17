#pragma once

#include <iostream>
#include <string>

namespace util
{
	enum LOG_LEVEL
	{
		INFO = 1,
		DEBUG = 2,
		WARN = 3,
		ERR = 4,
		PANIC = 5
	};

	extern const char* endl;

	class Logger
	{
	private:
		class InternalLogger
		{
		public:
			InternalLogger(const char* prefix);
			InternalLogger& operator<<(const char* msg);
			InternalLogger& operator<<(const std::string& msg);

			void setActive(bool isOn);
		private:
			bool _out;
			bool _shouldPrefix;
			const char* _prefix;
		};

	public:
		Logger(LOG_LEVEL defaultLogLevel, LOG_LEVEL minLevel, const char* prefix);
		void setMinLevel(LOG_LEVEL level);
		void setDefaultLevel(LOG_LEVEL defaultLevel);
		
		InternalLogger& operator<<(const char* msg);
		InternalLogger& operator<<(const std::string& msg);
		
		InternalLogger info;
		InternalLogger debug;
		InternalLogger warn;
		InternalLogger error;
		InternalLogger panic;

	private:
		LOG_LEVEL _defaultLevel;
		LOG_LEVEL _minLevel;
		const char* _prefix;
	};
}