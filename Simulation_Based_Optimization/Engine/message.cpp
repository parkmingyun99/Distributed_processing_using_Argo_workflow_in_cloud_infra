#include <ranges>

#include "message.h"
#include "sim_type.h"
#include "logger.h"
#include <utility>
#include <sstream>

Message::Message() = default;

Message::Message(const Time& time)
	: time_(time) {
}

Message::Message(const Time& time, BaseModel* src_model) :
	time_(time), src_model_(src_model) {
}

Message::~Message() = default;

const Time& Message::GetTime() const {
	return time_;
}

void Message::SetPortValue(const PortId& out_port, std::shared_ptr<void> value_ptr, const Time& delay) {
	port_value_to_delay_.emplace_back(std::make_pair(out_port, std::move(value_ptr)), delay);
}

void Message::SetSrcModel(BaseModel* base_model) {
	src_model_ = base_model;
}

BaseModel* Message::GetSrcModel() const {
	return src_model_;
}

void Message::SetPriority(const int& priority) {
	priority_ = priority;
}

PortValue Message::GetPortValuePair() const {
	if (port_value_to_delay_.size() != 1) {
		ELOG->critical("Port Value Size is not 1");
		return { kErrorPort, nullptr };
	}

	return port_value_to_delay_.front().first;
}

std::string Message::ToString() const {
	std::stringstream ss;
	ss << "[ t= " << time_ << ", in_ports = {";

	auto index = 0;
	for (const auto& [key, value] : port_value_to_delay_) {
		if (index++ == 0)
			ss << key.first;
		else
			ss << "," << key.second;
	}

	ss << "]";

	return ss.str();
}

PortId Message::InPort() const {
	return port_value_to_delay_.front().first.first;
}

void* Message::Value() const {
	return port_value_to_delay_.front().first.second.get();
}

BaseModel* Message::SrcModel() const {
	return src_model_;
}

bool Message::IsPriorityValid() const {
	return priority_ != kUnDefPriority;
}

int Message::Priority() const {
	return priority_;
}

void Message::ExtractPortValues(
	std::deque<std::pair<PortValue, Time>>& port_value_to_delay) {
	port_value_to_delay = std::move(port_value_to_delay_);
}
