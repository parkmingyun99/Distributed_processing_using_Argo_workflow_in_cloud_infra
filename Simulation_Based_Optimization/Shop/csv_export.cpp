#include "csv_export.h"
#include "logger.h"
#include <ranges>
#include <fstream>


std::unique_ptr<CsvExport> CsvExport::instance_ = nullptr;

CsvExport& CsvExport::Instance() {
	if (!instance_) {
		instance_ = std::unique_ptr<CsvExport>(new CsvExport());
	}
	return *instance_;
}

void CsvExport::Reset() {
	instance_.reset(new CsvExport);
}

void CsvExport::Enable() {
	en_log_ = true;
}

void CsvExport::RegisterTask(const TaskEntity& task) {
	if (en_log_) {
		saved_tasks_.push_back(task);
	}
}

void CsvExport::Export() {
	if (saved_tasks_.empty()) {
		return;
	}

	std::ofstream file_out;
	file_out.open("task.csv", std::ios::trunc);
	std::deque<std::string> col_vec{
		"Cell",
		"Start",
		"End",
		"Task",
		"Note"
	};
	file_out << fmt::format("{}\n", fmt::join(col_vec, ","));

	for (const auto& [name,start_cycle, end_cycle, task, note] : saved_tasks_) {
		// csv write
		std::deque rows = {
			name,
			std::to_string(start_cycle),
			std::to_string(end_cycle),
			task,
			note,
		};

		file_out << fmt::format("{}\n", fmt::join(rows, ","));
	}

	file_out.close();
}

CsvExport::CsvExport() = default;
