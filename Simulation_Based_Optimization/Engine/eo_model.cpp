#include "eo_model.h"
#include "sim_engine.h"
#include "logger.h"

EoModel::EoModel() : BaseModel(kEoAtomic) {
}

EoModel::~EoModel() = default;

bool EoModel::Initialize(Message& out_msg) {
	return true;
}

bool EoModel::TransFn(const Message& in_msg, Message& out_msg) {
	const auto in_port_id = in_msg.InPort();

	if (const auto iter = ev_handler_map_.find(in_port_id); iter != ev_handler_map_.end()) {
		return std::invoke((iter->second), in_msg, out_msg);
	}

	MLOG->critical("{} receives an event at unexpected port ({})", name_, in_port_id);
	return false;
}

void EoModel::NullifyPrevEvents() {
	EngInstance().priority_queue_.NullifyPrevEvents(this);
}

void EoModel::GetLastEvents(std::vector<BaseEv*>& ev_vec) {
	EngInstance().priority_queue_.GetPrevGenEvents(ev_vec, this);
}

BaseEv* EoModel::SendData(BaseModel* dest_model, const PortId& dest_in_port,
                          std::shared_ptr<void> value_ptr,
                          const Time& delay) {
	return SimEngine::Instance().SendData(this, dest_model, dest_in_port, std::move(value_ptr), delay);
}

BaseEv* EoModel::SendData(const PortId& src_out_port, std::shared_ptr<void> value_ptr,
                          const Time& delay) {
	return SimEngine::Instance().SendData(this, src_out_port, std::move(value_ptr), this->priority_, delay);
}

BaseEv* EoModel::SendData(BaseModel* dest_model, const PortId& dest_in_port, int priority,
                          std::shared_ptr<void> value_ptr,
                          const Time& delay) {
	return SimEngine::Instance().SendData(this, dest_model, priority, dest_in_port, std::move(value_ptr), delay);
}

Time EoModel::GetCurTime() {
	return EngInstance().GetCurClock();
}

void EoModel::RegisterEvHandler(const PortId& port_id,
                                const std::function<bool(const Message&, Message&)>& func) {
	ev_handler_map_[port_id] = func;

}


// template <typename Class> 
// void EoModel::RegisterEvHandler( const PortId& port_id, Class* ptr_object,
// 	bool( Class::* func)(const Message&, Message&) ) {
// 	using namespace std::placeholders;
// 	ev_handler_map_[port_id] = std::bind(func, ptr_object, _1, _2);
// }
