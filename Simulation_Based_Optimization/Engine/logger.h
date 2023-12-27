#pragma once
#include "lib_engine.h"
#include "spdlog/spdlog.h"

class SM_SIM_EXT_CLASS Logger {
public:
	static Logger& Instance();
	[[nodiscard]] bool IsLogStarted() const;
	//Start simulation mdl_name
	void StartSimLogger(const std::string& mdl_name);

	void StartOptLogger(const std::string& opt_name);
	//Stop the simulation logger 
	void StopSimLogger() const;
	//Stop the simulation logger 
	void StopConsoleLogger() const;

	void StopOptLogger() const;

	void Flush() const;

	//Console logger for simulation 
	[[nodiscard]] spdlog::logger* GetSimConsole() const;
	//File logger for simulation engine
	[[nodiscard]] spdlog::logger* GetEngineLogger() const;
	//File logger for model 
	[[nodiscard]] spdlog::logger* GetModelLogger() const;
	//File logger for model 
	[[nodiscard]] spdlog::logger* GetOptLogger() const;

	//Start the logger for engine debugging 
	void StartEngDebug() const;
	//Start the logger for engine debugging 
	void StartModelTrace() const;

	void StartOptTrace() const;

	Logger();
	~Logger();

private:
	inline static std::unique_ptr<Logger> instance_ = nullptr;

	std::shared_ptr<spdlog::logger> console_logger_{ nullptr };
	std::shared_ptr<spdlog::logger> eng_logger_{ nullptr };
	std::shared_ptr<spdlog::logger> model_logger_{ nullptr };
	std::shared_ptr<spdlog::logger> opt_logger_{ nullptr };
};

/**
 * \brief : console logger for model and simulation 
 */
#define CLOG Logger::Instance().GetSimConsole()
//File logger for simulation engine
#define ELOG Logger::Instance().GetEngineLogger()
//File logger for model 
#define MLOG Logger::Instance().GetModelLogger()

#define OLOG Logger::Instance().GetOptLogger()

inline Logger& LogInstance() { return Logger::Instance(); }
