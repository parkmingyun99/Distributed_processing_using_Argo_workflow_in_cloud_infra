#pragma once
#include "lib_sm_model.h"
#include "sim_type.h"
#include <memory>
#include <deque>
#include <string>

struct SM_MODEL_CLASS TaskEntity {
	std::string name{};
	Time start_cycle{ 0 };
	Time end_cycle{ 0 };
	std::string task{};
	std::string note{};
};

class SM_MODEL_CLASS CsvExport {
public:
	static CsvExport& Instance();

	// delete
	CsvExport(const CsvExport&) = delete;
	CsvExport& operator =(const CsvExport&) = delete;
	CsvExport(const CsvExport&&) = delete;
	CsvExport& operator =(const CsvExport&&) = delete;

	// register lot-processing tasks
	static void Reset();
	void Enable();
	void RegisterTask(const TaskEntity& task);

	// export times
	void Export();

	// lot process
	std::deque<TaskEntity> saved_tasks_;
	bool en_log_{ false };

private:
	CsvExport();
	static std::unique_ptr<CsvExport> instance_;
};
