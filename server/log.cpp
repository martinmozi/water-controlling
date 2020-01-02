#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/filesystem.hpp>
#include "log.h" 

std::string Log::logPath;

namespace
{
	std::shared_ptr<spdlog::logger> logger_;
    std::shared_ptr<spdlog::logger> consoleLogger_{ spdlog::stdout_color_mt("console") };
	spdlog::logger & logger()
	{
		if (!logger_)
		{
    		if (Log::logPath.empty())
    		{
        		printf("Missing log path");
        		exit(-1);
    		}

    		boost::system::error_code ec;
    		if (!boost::filesystem::is_directory(Log::logPath, ec))
        		boost::filesystem::create_directories(Log::logPath);

    		logger_ = spdlog::daily_logger_st("daily_logger", Log::logPath + "/daily.log");
			logger_->flush_on(spdlog::level::debug);
		}
		
		return *(logger_.get());
	}

	#define logMacro  \
	{ \
		char buffer[1024]; \
		va_list myargs; \
		va_start(myargs, __fmt); \
		vsnprintf(buffer, sizeof(buffer), __fmt, myargs); \
		va_end(myargs); \
		bufferStr += buffer; \
	}
}

namespace Log
{

    void error(const char *__fmt, ...)
    {
        std::string bufferStr;
        logMacro;
        logger().error(bufferStr);
        consoleLogger_->error(bufferStr);
    }

    void warning(const char *__fmt, ...)
    {
        std::string bufferStr;
        logMacro;
        logger().warn(bufferStr);
        consoleLogger_->warn(bufferStr);
    }

    void info(const char *__fmt, ...)
    {
        std::string bufferStr;
        logMacro;
        logger().info(bufferStr);
        consoleLogger_->info(bufferStr);
    }
}
