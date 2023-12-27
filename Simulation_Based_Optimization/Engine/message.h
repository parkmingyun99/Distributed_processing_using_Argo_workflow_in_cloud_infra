#pragma once

#include "lib_engine.h"
#include "sim_type.h"
#include "base_model.h"

#include <deque>
#include <string>
#include <memory>

using PortValue = std::pair<unsigned int, std::shared_ptr<void>>;

class SM_SIM_EXT_CLASS Message {
public:
	Message();

	explicit Message(const Time& time);

	explicit Message(const Time& time, BaseModel* src_model);

	~Message();

	//get generation wait_time of message
	[[nodiscard]] const Time& GetTime() const;

	void SetPortValue(const PortId& out_port, std::shared_ptr<void> value_ptr, const Time& delay = 0);

	//    template<typename T>
	//    void SetPortValue(const PortId &out_port, T *value_ptr, const Time &delay = 0) {
	//        return SetPortValue(out_port, std::shared_ptr<T>(value_ptr), delay);
	//    }
	template <typename T>
	void SetPortValue(const PortId& out_port, T* value_ptr, const Time& delay = 0) {
		SetPortValue(out_port, std::shared_ptr<T>(value_ptr), delay);
	}

	void SetSrcModel(BaseModel* base_model);

	[[nodiscard]] BaseModel* GetSrcModel() const;

	void SetPriority(const int& priority);

	//get a src_port num of first interval
	[[nodiscard]] PortValue GetPortValuePair() const;

	[[nodiscard]] PortId InPort() const;

	[[nodiscard]] void* Value() const;

	[[nodiscard]] BaseModel* SrcModel() const;

	[[nodiscard]] bool IsPriorityValid() const;

	[[nodiscard]] int Priority() const;

	//called by the engine to get the src_port-value pairs
	void ExtractPortValues(std::deque<std::pair<PortValue, Time>>& port_value_to_delay);

	//wait_time and port pair to string for debugging
	[[nodiscard]] std::string ToString() const;

private:
	Time time_{ kUnknownTime };
	BaseModel* src_model_{ nullptr };
	int priority_{ kUnDefPriority };
	std::deque<std::pair<PortValue, Time>> port_value_to_delay_;
};
