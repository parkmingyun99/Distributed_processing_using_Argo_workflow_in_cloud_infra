#include "sim_events.h"

ModelSchEv::ModelSchEv(const Time& time, BaseModel* src_model) :
	BaseEv(time, EvType::INT_EV_TYPE, src_model, src_model, src_model->GetPriority()) {
}

ModelSchEv::~ModelSchEv() = default;

CoupChEv::~CoupChEv() = default;

CoupChEv::CoupChEv(const Time& time, BaseModel* src_model, const int& priority, std::shared_ptr<void>& msg_ptr) :
	BaseEv(time, EvType::COUP_CHANGE_TYPE, src_model, src_model, priority), msg_ptr_(std::move(msg_ptr)) {
}


void* CoupChEv::GetMsg() const { return msg_ptr_.get(); }

std::shared_ptr<void>& CoupChEv::GetMsg() { return msg_ptr_; }

ExtEv::ExtEv(const Time& time, BaseModel* src_model, const PortId& src_out_port, BaseModel* dest_model,
             const PortId& dest_in_port, std::shared_ptr<void>& msg_ptr) :
	BaseEv(time, EvType::EXT_EV_TYPE, src_model, src_out_port, dest_model, dest_in_port),
	msg_ptr_{ std::move(msg_ptr) } {
}

ExtEv::ExtEv(const Time& time, BaseModel* src_model, const PortId& src_out_port, BaseModel* dest_model,
             const PortId& dest_in_port, const int& priority, std::shared_ptr<void>& msg_ptr) :
	BaseEv(time, EvType::EXT_EV_TYPE, src_model, src_out_port, dest_model, dest_in_port, priority),
	msg_ptr_(std::move(msg_ptr)) {
}


void* ExtEv::GetMsg() const {
	return msg_ptr_.get();
}

std::shared_ptr<void>& ExtEv::GetMsg() {
	return msg_ptr_;
}

ExtEv* ExtEv::GetClone() const {
	auto* const ret_ev = new ExtEv(*this);
	return ret_ev;
}

ExtEv::~ExtEv() = default;

PortId ExtEv::GetSrcOutPort() const {
	return out_port_;
}

PortId ExtEv::GetDestInPort() const {
	return in_port_;
}
