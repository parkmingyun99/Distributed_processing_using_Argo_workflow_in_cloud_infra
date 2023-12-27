#pragma once

#include <map>
#include "lib_engine.h"
#include "base_model.h"
#include "message.h"
#include "base_ev.h"
#include "sim_events.h"

#include <functional>
#include <memory>

//template <typename ModelClass>
//using EvHandlerType = std::function<bool(ModelClass&, const Message&, Message&)>;

class SM_SIM_EXT_CLASS EoModel : public BaseModel {
public:
	EoModel();

	~EoModel() override;

	// state transition
	virtual bool Initialize(Message& out_msg);

	virtual bool TransFn(const Message& in_msg, Message& out_msg);

	// Handling the previous generated events
	void NullifyPrevEvents();

	void GetLastEvents(std::vector<BaseEv*>& ev_vec);

	// Send direct interval
	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, std::shared_ptr<void> value_ptr,
	                 const Time& delay = 0);

	template <typename T>
	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, T* value_ptr, const Time& delay = 0) {
		return SendData(dest_model, dest_in_port, std::shared_ptr<T>(value_ptr), delay);
	}

	BaseEv* SendData(const PortId& src_out_port, std::shared_ptr<void> value_ptr, const Time& delay = 0);

	template <typename T>
	BaseEv* SendData(const PortId& src_out_port, T* value_ptr, const Time& delay = 0) {
		return SendData(src_out_port, std::shared_ptr<T>(value_ptr), delay);
	}

	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, int priority, std::shared_ptr<void> value_ptr,
	                 const Time& delay = 0);

	template <typename T>
	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, int priority, T* value_ptr,
	                 const Time& delay = 0) {
		return SendData(dest_model, dest_in_port, priority, std::shared_ptr<T>(value_ptr), delay);
	}

	template <typename T>
	BaseEv* SendCoupleChange(T* value_ptr, const Time& delay = 0) {
		return SendData(nullptr, static_cast<PortId>(EvType::COUP_CHANGE_TYPE), std::shared_ptr<T>(value_ptr), delay);
	}

	static Time GetCurTime();


	void RegisterEvHandler(const PortId& port_id,
	                       const std::function<bool(const Message&, Message&)>& func);

protected:
	/* event handler map */
	std::map<PortId, std::function<bool(const Message&, Message&)>> ev_handler_map_;

};
