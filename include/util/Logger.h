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
			InternalLogger(std::string prefix);
			InternalLogger& operator<<(const char* msg);
			InternalLogger& operator<<(const std::string& msg);
			InternalLogger& operator<<(std::uint32_t t);

			void setActive(bool isOn);
		private:
			bool _out;
			bool _shouldPrefix;
			std::string prefix_;
		};

	public:
		Logger(LOG_LEVEL defaultLogLevel, LOG_LEVEL minLevel, std::string prefix);
		void setMinLevel(LOG_LEVEL level);
		void setDefaultLevel(LOG_LEVEL defaultLevel);
		
		InternalLogger& operator<<(const char* msg);
		InternalLogger& operator<<(const std::string& msg);
		InternalLogger& operator<<(std::uint32_t t);
		
		InternalLogger info;
		InternalLogger debug;
		InternalLogger warn;
		InternalLogger error;
		InternalLogger panic;

	private:
		LOG_LEVEL _defaultLevel;
		LOG_LEVEL _minLevel;
		std::string prefix_;
	};
}