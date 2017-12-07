#include <util/Logger.h>

namespace util
{
	const char* endl = "\n";

	Logger::Logger(LOG_LEVEL defaultLogLevel, LOG_LEVEL minLevel, const char* prefix)
		: info(prefix)
		, debug(prefix)
		, warn(prefix)
		, error(prefix)
		, panic(prefix)
		, _prefix(prefix)
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

	Logger::InternalLogger::InternalLogger(const char* prefix)
		: _out(true)
		, _shouldPrefix(true)
		, _prefix(prefix)
	{}

	Logger::InternalLogger& Logger::InternalLogger::operator<<(const char* msg)
	{
		if (!_out)
		{
			return *this;
		}

		for (const char* s = msg; s != nullptr && *s != '\0'; s++)
		{
			if (_shouldPrefix && _prefix != nullptr)
			{
				std::cout << _prefix;
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

	void Logger::InternalLogger::setActive(bool isOn)
	{
		_out = isOn;
	}
}