#pragma once

#include "lib_engine.h"
#include "sim_type.h"
#include "message.h"
#include "base_model.h"
#include "base_ev.h"

class SM_SIM_EXT_CLASS DevsAtomic : public BaseModel {
public:
	DevsAtomic();

	explicit DevsAtomic(const std::string& name);

	~DevsAtomic() override;

	virtual bool ExtTransFn(const Message& msg) { return false; };

	virtual bool IntTransFn() { return false; }

	virtual bool OutputFn(Message& msg) { return false; }

	virtual Time TimeAdvanceFn() { return -1; }

	// Event nullification
	void Continue();

	void SetLastSchEv(BaseEv* ptr_ev) { last_gen_ev_ptr_ = ptr_ev; }

	[[nodiscard]] BaseEv* GetLastSchEv() const { return last_gen_ev_ptr_; }

	bool GetContinueValue();

	BaseEv* last_gen_ev_ptr_ = nullptr;


	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port,
	                 std::shared_ptr<void> value_ptr,
	                 const Time& delay = 0);

	template <typename T>
	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, T* value_ptr,
	                 const Time& delay = 0) {
		return SendData(dest_model, dest_in_port, std::shared_ptr<T>(value_ptr), delay);
	}

	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port,
	                 std::shared_ptr<void> value_ptr,
	                 int priority, const Time& delay);

	template <typename T>
	BaseEv* SendData(BaseModel* dest_model, const PortId& dest_in_port, T* value_ptr,
	                 int priority, const Time& delay = 0) {
		return SendData(dest_model, dest_in_port, std::shared_ptr<T>(value_ptr), priority, delay);
	}


private:
	bool continue_{ false };
};
