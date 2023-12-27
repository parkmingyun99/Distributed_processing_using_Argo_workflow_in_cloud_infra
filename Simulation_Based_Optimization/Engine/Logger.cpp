#include "logger.h"
#include <chrono>
#include <sstream>
#include <iomanip>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

Logger::Logger() = default;

Logger::~Logger() {
	Flush();
}

Logger& Logger::Instance() {
	if (!instance_) {
		instance_ = std::make_unique<Logger>();
	}
	return *instance_;
}

bool Logger::IsLogStarted() const {
	return eng_logger_ != nullptr;
}

void Logger::StartSimLogger(const std::string& mdl_name) {
	const auto n = std::chrono::system_clock::now();
	const auto in_time_t = std::chrono::system_clock::to_time_t(n);

	/* console */
	console_logger_ = spdlog::stdout_color_mt(mdl_name);

	std::stringstream ss;
	ss << mdl_name << "_"
		<< std::put_time(std::localtime(&in_time_t), "%b_%d_%H_%M_%S") << ".log";
	//<< "_" << num_run_ << ".log";

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(ss.str(), true);

	model_logger_ = std::make_shared<spdlog::logger>("MOD", file_sink);
	eng_logger_ = std::make_unique<spdlog::logger>("ENG", file_sink);

	model_logger_->set_pattern("M %5l| %v");
	eng_logger_->set_pattern("E %5l| %v");
}

void Logger::StartOptLogger(const std::string& opt_name) {
	const auto n = std::chrono::system_clock::now();
	const auto in_time_t = std::chrono::system_clock::to_time_t(n);

	std::stringstream ss;
	ss << opt_name << "_"
		<< std::put_time(std::localtime(&in_time_t), "%b_%d_%H_%M_%S") << ".log";

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(ss.str(), true);

	opt_logger_ = std::make_unique<spdlog::logger>("OPT", file_sink);
	opt_logger_->set_pattern("O %5l| %v");
}

void Logger::StopSimLogger() const {
	//log off 
	if (console_logger_) { console_logger_->set_level(spdlog::level::off); }

	if (eng_logger_) { eng_logger_->set_level(spdlog::level::off); }

	if (model_logger_) { model_logger_->set_level(spdlog::level::off); }
}

void Logger::StopConsoleLogger() const {
	if (console_logger_) { console_logger_->set_level(spdlog::level::off); }
}

void Logger::StopOptLogger() const {
	if (opt_logger_) { opt_logger_->set_level(spdlog::level::off); }
}

void Logger::Flush() const {
	if (console_logger_) { console_logger_->flush(); }

	if (eng_logger_) { eng_logger_->flush(); }

	if (model_logger_) { model_logger_->flush(); }

	if (opt_logger_) { opt_logger_->flush(); }
}

spdlog::logger* Logger::GetSimConsole() const { return console_logger_.get(); }

spdlog::logger* Logger::GetEngineLogger() const { return eng_logger_.get(); }

spdlog::logger* Logger::GetModelLogger() const { return model_logger_.get(); }

spdlog::logger* Logger::GetOptLogger() const {
	return opt_logger_.get();
}


void Logger::StartEngDebug() const {
	if (eng_logger_) {
		eng_logger_->set_level(spdlog::level::debug);
	}

	if (model_logger_) {
		model_logger_->set_level(spdlog::level::debug);
	}
}

void Logger::StartModelTrace() const {
	if (model_logger_) {
		model_logger_->set_level(spdlog::level::trace);
	}
}

void Logger::StartOptTrace() const {
	if (opt_logger_) {
		opt_logger_->set_level(spdlog::level::trace);
	}
}
