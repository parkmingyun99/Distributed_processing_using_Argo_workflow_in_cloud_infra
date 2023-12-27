#pragma once

#include "base_model.h"
#include "message.h"
#include "base_ev.h"
#include "coup_model.h"
#include "sm_priority_queue.h"
#include "base_dyn_coup_handler.h"

#include <thread>
#include "sim_events.h"
#include "date/date.h"

class SM_SIM_EXT_CLASS SimEngine {
public:
	static SimEngine& Instance();

	static void SetEngDebugMode();

	static void SetModelTraceMode();

	static void StopLog();

	void StopConsoleLog() const;

	void SetModel(CoupModel* ptr_model);

	[[nodiscard]] CoupModel* GetModel() const;

	void Reset();

	void Reset(CoupModel* coup_model);

protected: //private functions called by the engine
	bool run();

	//schedule initial internal events of DEVS Atomic models
	void InitAtomicModels(CoupModel*);

	/**
	 * \brief : message conversion function for modeling convenience
	 * \param conv_in_msg : converted message from event
	 * \param ext_ev : source event
	 */
	void ConvertToMessage(Message& conv_in_msg, ExtEv* ext_ev) const;

	void ConvertToMessage(Message& conv_in_msg, CoupChEv* coup_ch_ev) const;

	/**
	 * \brief : Convert messages to events
	 * \param src_model : Event-generated models
	 * \param out_msg : Containing generated all data
	 */
	void DeliverOutMessages(BaseModel* src_model, Message& out_msg);

	//handle internal events during run()
	bool HandleIntEvent(BaseEv* ptr_ev);

	//handle external events during run()
	bool HandleExtEvent(BaseEv* ptr_ev);

	//handle external events during run()
	bool HandleCoupChEvent(BaseEv* ptr_ev) const;

public: //runtime simulation control 

	bool Start();

	BaseEv* MakeEvent(BaseModel* src_model, BaseModel* dest_model, const PortId& in_port,
	                  std::shared_ptr<void>&& value_ptr, const int& priority, const Time& delay) const;

	//modeling API
	/**
	 * \brief k
	 * \param src_model: event sender
	 * \param dest_model: event receiver
	 * \param in_port: input port
	 * \param value_ptr: value pointer
	 * \param delay: delay from current cycle
	 */
	BaseEv*
	SendData(BaseModel* src_model, BaseModel* dest_model, const PortId& in_port,
	         std::shared_ptr<void>&& value_ptr, const Time& delay = 0);

	BaseEv*
	SendData(BaseModel* src_model, BaseModel* dest_model, const int& priority, const PortId& in_port,
	         std::shared_ptr<void>&& value_ptr, const Time& delay);

	[[nodiscard]] const Time& GetCurClock() const;

	[[nodiscard]] const date::sys_seconds& GetStartDate() const;

	[[nodiscard]] const date::sys_seconds& GetCurDate() const;

	[[nodiscard]] const date::sys_seconds& GetEndDate() const;

	void SetCurClock(const Time& cur_clock);

	//void SetDT(UdeBaseModel *_this, double _duration);
	//UdeBaseModel* findComponent(std::string _name);

	/**
	 * \API to be called by engine after ta
	 * \param ta: remained reach_time to be called
	 * \param src_model: model to be called
	 * \return
	 */
	void ScheduleDevAtomic(const Time& ta, BaseModel* src_model);

	/**
	 * \API to transfer message
	 * \param delay: scheduled reach_time called by engine
	 * \param src_model: ptr_msg_value sending model
	 * \param out_port: ptr_msg_value sending port
	 * \param priority: priority
	 * \param ptr_msg_value: contained values
	 */

	BaseEv*
	SendData(BaseModel* src_model, const PortId& out_port, std::shared_ptr<void>&& ptr_msg_value,
	         const int& priority, const Time& delay);

	BaseEv* ScheduleEvent(ExtEv* ext_ev);


	/**
	 * \brief : Show the overall model information
	 */

	// Dynamic coupling
	void SetDynCoupHandler(BaseDynCoupHandler* coup_handler);

	[[nodiscard]] BaseDynCoupHandler* GetDynCoupHandler() const;

	void ShowModelInfo() const;

	void ShowModelInfo(CoupModel* coup_model) const;

#ifdef EN_EV_COUNT

	[[nodiscard]] int GetEvNum() const { return num_ev_; }

#endif

	// queue locker for the extension to parallel simulation
	SmPriorityQueue priority_queue_;

private:
	Time cur_cycle_ = 0;
	Time end_cycle_ = kInfinity;

	[[nodiscard]] std::pair<std::string, std::string> GetPortIdStrPair(
		const PortId& out_port_id, const std::string& out_port_str) const;

	// out-most coupled
	std::unique_ptr<CoupModel> initial_model_{};

	// coupling handler
	std::unique_ptr<BaseDynCoupHandler> dyn_coup_handler_{};

#ifdef EN_EV_COUNT
	int num_ev_{ 0 }; // number of the processed event
#endif
	int num_run_{ 0 };

public:
	SimEngine();

	~SimEngine();

};

extern SM_SIM_EXT_CLASS inline SimEngine& EngInstance() {
	return SimEngine::Instance();
};
