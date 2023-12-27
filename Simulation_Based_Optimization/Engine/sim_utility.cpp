#include "sim_utility.h"

date::sys_seconds Convert(const std::string& date_time_str) {
	date::sys_seconds result;
	std::istringstream iss(date_time_str);
	iss >> date::parse("%d/%m/%Y %H:%M:%S", result);
	// iss >> date::parse("%d/%m/%Y %H:%M:%S", result);
	assert(result != date::sys_seconds{});
	return result;
}

std::string GetString(const date::sys_seconds& time_var) {
	//return date::format("%d/%m/%Y %H:%M:%S", time_var);
	return date::format("%d/%m/%Y %H:%M:%S", time_var);
}

std::string GetString(const ModelType& model_type) {
	switch (model_type) {
	case kCoupled: return std::string("COUPLE");
	case kDevsAtomic: return std::string("DEVS");
	case kEoAtomic: return std::string("EO");
	default: ;
		return std::string("Unknown");
	}
}

std::string GetModelName(const BaseModel* model) {
	if (model != nullptr) {
		return model->GetName();
	}

	return std::string("<null>");
}

std::string GetInPortId(BaseModel* model, const int& in_port) {
	if (model != nullptr) {
		return model->GetInPortName(in_port);
	}

	return std::string("<null>");
}

std::string GetStringFromMsg(const void* ptr_msg) {
	const auto* const address = ptr_msg;
	std::stringstream ss;
	ss << address;
	return ss.str();
}
