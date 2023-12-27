#include "base_ev.h"
#include <stdexcept>
#include "logger.h"
#include "sim_utility.h"

BaseEv::BaseEv(const Time& time, const EvType& type,
               BaseModel* src_model, BaseModel* dest_model, const int& priority):
	time_(time), ev_type_(type), src_model_(src_model), dest_model_(dest_model),
	priority_(priority) {
}

BaseEv::BaseEv(const Time& time, const EvType& type,
               BaseModel* src_model, const PortId& out_port,
               BaseModel* dest_model, const PortId& in_port, const int& priority):
	time_(time), ev_type_(type), src_model_(src_model), out_port_(out_port),
	dest_model_(dest_model), in_port_(in_port), priority_(priority) {
}

/** * \brief */
BaseEv::BaseEv() = default;

BaseEv::~BaseEv() = default;

void BaseEv::IncreaseTime(const Time& delay) { time_ += delay; }

void BaseEv::Nullify() {
	active_ = false;
#if !defined(NDEBUG) || defined(ENG_LOG_TRACE_ENABLE)
	ELOG->debug(" ! nullified: {}", GetStringFromMsg(this));
#endif
}


const EvType& BaseEv::GetEvType() const { return ev_type_; }

BaseModel* BaseEv::GetSrcModel() const { return src_model_; }

BaseModel* BaseEv::GetDestModel() const { return dest_model_; }

std::list<std::pair<BaseModel*, int>>& BaseEv::GetDestPortList() const {
	if (src_model_ == nullptr) {
		throw std::runtime_error("BaseEv::GetDestPortList is called");
	}
	return src_model_->GetDestModelPortList(out_port_);
}

std::string BaseEv::GetSrcPortName() const {

	if (src_model_ != nullptr) {
		return src_model_->GetName() + ": " + std::to_string(out_port_) + "/" +
			src_model_->GetOutPortName(out_port_);
	}

	return "null: " + std::to_string(out_port_) + "/unknown";
}

void BaseEv::SetDestInPort(BaseModel* dest_model, const PortId& in_port) {
	dest_model_ = dest_model;
	in_port_ = in_port;
}

void BaseEv::SetSrcOutPort(BaseModel* src_model, const PortId& out_port) {
	src_model_ = src_model;
	out_port_ = out_port;
}

void BaseEv::SetPriority(const int& priority) { priority_ = priority; }

int BaseEv::GetPriority() const { return priority_; }
