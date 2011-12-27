#ifndef SF_LOGGER_HPP
#define SF_LOGGER_HPP

#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <utility/logger/exception.hpp>
#include <boost/noncopyable.hpp>

#include <boost/date_time.hpp>
namespace pt = boost::posix_time;

namespace sf
{
namespace utility
{

class Logger : boost::noncopyable
{
public:
	enum LogLevel {ERROR_L = 1, WARN_L, DEBUG_L, INFO_L};

	Logger(const std::string &outputPath, LogLevel l = ERROR_L)
	:filePath(outputPath), level(l)
	, timestampFormat("%a, %d %b %Y %H:%M:%s")
	{
		file = fopen(outputPath.c_str(), "a+");
		if(!file)
			throw InvalidLogException() 
				<< errorMessage("Failed to open log file '" + outputPath + "'");
	}

	//Free-Style Log! Logs whatever data is provided,
	// does not add timestamp or log level
	//Throws: InvalidLogException
	void operator()(const char *format, ...)
	{
		va_list va;

		va_start(va, format);
		if(vfprintf(file, format, va) < 0)
			throw InvalidLogException()
				<< errorMessage("Failed to write to log file '" + filePath +"'");
		va_end(va);
	}

	//Logs "[timestamp] [level] provided-log-data"
	//Default timestamp format is 'Sun, 06 Nov 1994 08:49:37'
	//Throws: InvalidLogException
	void operator()(LogLevel l, const char *format, ...)
	{
		//Do not log messages with a higher report level
		//than the level set for the logger
		if(l <= level)
		{
			pt::ptime time(pt::microsec_clock::local_time());
			std::stringstream ss;
			pt::time_facet *facet = new pt::time_facet(timestampFormat.c_str());
			
			ss.imbue(std::locale(ss.getloc(), facet));
			ss << time;
			if(fprintf(file, "[%s] [%s] ", ss.str().c_str(), logLevelString(l).c_str()) < 0)
				throw InvalidLogException() 
					<< errorMessage("Failed to write to log file '" + filePath +"'");

			va_list va;

			va_start(va, format);
			if(vfprintf(file, format, va) < 0)
				throw InvalidLogException() 
					<< errorMessage("Failed to write to log file '" + filePath +"'");
			va_end(va);
		}
	}

	LogLevel logLevel() const
	{
		return level;
	}

	std::string outputPath() const
	{
		return filePath;
	}

	std::string logLevelString(LogLevel l) const
	{
		switch(l)
		{
			case ERROR_L:
				return "error";
			case WARN_L:
				return "warning";
			case DEBUG_L:
				return "debug";
			case INFO_L:
				return "info";
			default:
				return "unknown level";
		}
	}

	~Logger()
	{
		if(file)
			fclose(file);
	}

private:
	FILE *file;

	std::string filePath;

	LogLevel level;

	std::string timestampFormat;
};

}
}

#endif