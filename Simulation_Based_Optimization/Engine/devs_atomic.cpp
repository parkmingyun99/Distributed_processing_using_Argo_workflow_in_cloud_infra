#include "devs_atomic.h"
#include "sim_engine.h"

DevsAtomic::DevsAtomic() : BaseModel(kDevsAtomic) {
}

DevsAtomic::DevsAtomic(const std::string& name)
	: BaseModel(name, kDevsAtomic) {
}

DevsAtomic::~DevsAtomic() = default;


void DevsAtomic::Continue() {
	continue_ = true;
}

bool DevsAtomic::GetContinueValue() {
	if (continue_) {
		continue_ = false;
		return true;
	}
	return false;
}

BaseEv* DevsAtomic::SendData(BaseModel* dest_model, const PortId& dest_in_port,
                             std::shared_ptr<void> value_ptr, const Time& delay) {
	return EngInstance().SendData(this, dest_model, dest_in_port, std::move(value_ptr), delay);
}

BaseEv* DevsAtomic::SendData(BaseModel* dest_model, const PortId& dest_in_port,
                             std::shared_ptr<void> value_ptr, const int priority, const Time& delay) {
	return EngInstance().SendData(this, dest_model, priority, dest_in_port, std::move(value_ptr), delay);
}
