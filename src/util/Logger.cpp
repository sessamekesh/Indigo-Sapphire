#include <util/Logger.h>

namespace util
{
	const char* endl = "\n";

	Logger::Logger(LOG_LEVEL defaultLogLevel, LOG_LEVEL minLevel, std::string prefix)
		: info(prefix)
		, debug(prefix)
		, warn(prefix)
		, error(prefix)
		, panic(prefix)
		, prefix_(prefix)
	{
		setMinLevel(minLevel);
		setDefaultLevel(defaultLogLevel);
	}

	void Logger::setMinLevel(LOG_LEVEL minLevel)
	{
		_minLevel = minLevel;

		info.setActive(minLevel <= INFO);
		debug.setActive(minLevel <= DEBUG);
		warn.setActive(minLevel <= WARN);
		error.setActive(minLevel <= ERR);
		panic.setActive(minLevel <= PANIC);
	}

	void Logger::setDefaultLevel(LOG_LEVEL defaultLevel)
	{
		_defaultLevel = defaultLevel;
	}

	Logger::InternalLogger& Logger::operator<<(const char* msg)
	{
		switch (_defaultLevel)
		{
		case INFO: info << msg; return info;
		case DEBUG: debug << msg; return debug;
		case WARN: warn << msg; return warn;
		case ERR: error << msg; return error;
		default: case PANIC: panic << msg; return panic;
		}
	}

	Logger::InternalLogger& Logger::operator<<(const std::string& msg)
	{
		switch (_defaultLevel)
		{
		case INFO: info << msg; return info;
		case DEBUG: debug << msg; return debug;
		case WARN: warn << msg; return warn;
		case ERR: error << msg; return error;
		default: case PANIC: panic << msg; return panic;
		}
	}

	Logger::InternalLogger& Logger::operator<<(std::uint32_t msg)
	{
		switch (_defaultLevel)
		{
		case INFO: info << msg; return info;
		case DEBUG: debug << msg; return debug;
		case WARN: warn << msg; return warn;
		case ERR: error << msg; return error;
		default: case PANIC: panic << msg; return panic;
		}
	}

	Logger::InternalLogger::InternalLogger(std::string prefix)
		: _out(true)
		, _shouldPrefix(true)
		, prefix_(prefix)
	{}

	Logger::InternalLogger& Logger::InternalLogger::operator<<(const char* msg)
	{
		if (!_out)
		{
			return *this;
		}

		for (const char* s = msg; s != nullptr && *s != '\0'; s++)
		{
			if (_shouldPrefix)
			{
				std::cout << prefix_;
				_shouldPrefix = false;
			}

			if (*s == '\n')
			{
				_shouldPrefix = true;
				std::cout.flush();
			}

			std::cout << *s;
		}

		return *this;
	}

	Logger::InternalLogger& Logger::InternalLogger::operator<<(const std::string& msg)
	{
		return this->operator<<(msg.c_str());
	}

	Logger::InternalLogger& Logger::InternalLogger::operator<<(std::uint32_t t)
	{
		if (!_out)
		{
			return *this;
		}

		if (_shouldPrefix)
		{
			std::cout << prefix_;
			_shouldPrefix = false;
		}

		std::cout << t;
		return *this;
	}

	void Logger::InternalLogger::setActive(bool isOn)
	{
		_out = isOn;
	}
}