#pragma once

#include "lib_engine.h"
#include "sim_type.h"
#include "base_model.h"

class SM_SIM_EXT_CLASS BaseEv {
public:
	BaseEv(const Time& time, const EvType& type, BaseModel* src_model,
	       BaseModel* dest_model, const int& priority);

	BaseEv(const Time& time, const EvType& type, BaseModel* src_model, const PortId& out_port,
	       BaseModel* dest_model, const PortId& in_port, const int& priority = kUnDefPriority);

	BaseEv();

	BaseEv(const BaseEv& rhs) = default;

	virtual ~BaseEv();

	// delete
	BaseEv& operator=(const BaseEv&) = delete;

	BaseEv(const BaseEv&&) = delete;

	BaseEv& operator=(const BaseEv&&) = delete;

	[[nodiscard]] Time GetTime() const { return time_; };

	void SetTime(const Time& ev_time) {
		time_ = ev_time;
	};

	void IncreaseTime(const Time& delay);

	[[nodiscard]] const EvType& GetEvType() const;

	// model destination
	[[nodiscard]] BaseModel* GetSrcModel() const;

	[[nodiscard]] BaseModel* GetDestModel() const;

	[[nodiscard]] std::list<std::pair<BaseModel*, int>>& GetDestPortList() const;

	[[nodiscard]] std::string GetSrcPortName() const;

	void SetDestInPort(BaseModel* dest_model, const PortId& in_port);

	void SetSrcOutPort(BaseModel* src_model, const PortId& out_port);

	// Event nullification
	[[nodiscard]] bool IsActive() const { return active_; }

	void Nullify();

	// set priority
	void SetPriority(const int& priority);

	[[nodiscard]] inline int GetPriority() const;

	Time time_{ kUnknownTime };
	EvType ev_type_{ EvType::EV_TYPE_UNKNOWN };
	BaseModel* src_model_{ nullptr };
	PortId out_port_{ kUnknownPort };
	BaseModel* dest_model_{ nullptr };
	PortId in_port_{ kUnknownPort };
	int priority_{ kUnDefPriority };
	bool active_{ true };
};
