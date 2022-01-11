#include "Client.hpp"
#include <easylogging++.h>

INITIALIZE_EASYLOGGINGPP

void initLogger() {
    el::Configurations loggerConfiguration;
    //el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%startTime", std::bind(getTimeSinceProgramStart)));
    //std::string format = "%s [%startTime][%level][%thread][%fbase]: %msg";

    // https://github.com/amrayn/easyloggingpp#datetime-format-specifiers
    // [%fbase:L%line]
    std::string format = "[%datetime{%H:%m:%s}] [%thread thread/%level]: %msg";
    loggerConfiguration.set(el::Level::Info, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Error, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Fatal, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Warning, el::ConfigurationType::Format, format);
    loggerConfiguration.set(el::Level::Debug, el::ConfigurationType::Format, format);
    el::Helpers::setThreadName("main");
    el::Loggers::reconfigureAllLoggers(loggerConfiguration);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    //el::Loggers::file
    LOG(INFO) << "Logger is configured";
}

int main(int argc, char** argv)
{
    initLogger();

    Client::RunClient();

    //World w("myworld.bin");
    ////w.GenerateHeader("1.0.0", "my world");
    //w.LoadHeaderSection("1.0.0", "myworld");
    //std::this_thread::sleep_for(5s);
    //w.Save();

	/**
    * Could write each world data section in portions
    * 
	*/
	
	return 0;
}