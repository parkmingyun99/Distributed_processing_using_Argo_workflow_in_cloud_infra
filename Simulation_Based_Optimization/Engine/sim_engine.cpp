#include "logger.h"
#include "sim_engine.h"
#include "sim_events.h"
#include "model_coupler.h"
#include "devs_atomic.h"
#include "eo_model.h"
#include "sim_utility.h"

#include <utility>
#include <chrono>

SimEngine& SimEngine::Instance() {
	static const std::unique_ptr<SimEngine> instance{ new SimEngine() };
	return *instance;
}

void SimEngine::SetEngDebugMode() {
	// if (initial_model_ == nullptr) {
	// 	throw std::runtime_error("a model need to be assigned before SetEngDebugMode");
	// }

	ELOG->info("** Enable eng debug mode ..");
	Logger::Instance().StartEngDebug();
}

void SimEngine::SetModelTraceMode() {
	// if (initial_model_ == nullptr) {
	// 	throw std::runtime_error("a model need to be assigned before SetModelTraceMode");
	// }
	//
	ELOG->info("** Enable mdl trace mode ..");
	Logger::Instance().StartModelTrace();
}

void SimEngine::StopLog() {
	// if (initial_model_ == nullptr) {
	// 	throw std::runtime_error("a model need to be assigned before StopLog");
	// }
	Logger::Instance().StopSimLogger();
}

void SimEngine::StopConsoleLog() const {
	if (initial_model_ == nullptr) {
		throw std::runtime_error("a model need to be assigned before StopConsoleLog");
	}

	Logger::Instance().StopConsoleLogger();
}

void SimEngine::SetModel(CoupModel* ptr_model) {
	const auto& model_name = ptr_model->GetName();
	/* start simulation log */
	if (!Logger::Instance().IsLogStarted()) {
		Logger::Instance().StartSimLogger(model_name);
	}

	ELOG->debug("** Initialize the model: {}", model_name);

	initial_model_.reset(ptr_model);
	// initial_model_ = std::move(ptr_model);

	/* make components and coupling */
	ModelCoupler coupler;
	if (!coupler.FlattenModel(initial_model_.get())) {
		ELOG->warn("Unconnected in/out ports exist");
	}

	// Load the change-enable coupling
	if (dyn_coup_handler_) {
		dyn_coup_handler_->InitFlagToSrcOutPorts(coupler.flag_to_src_out_port_pairs_);
	}
}

CoupModel* SimEngine::GetModel() const {
	if (initial_model_) {
		return initial_model_.get();
	}

	return nullptr;
}

void SimEngine::Reset() {
	//Reset current model
	initial_model_.reset(nullptr);
	// dyn_coup_handler_.reset(nullptr);
	priority_queue_.clear();
}

void SimEngine::Reset(CoupModel* coup_model) {
	//Reset current model
	Reset();
	// reset the new model
	SetModel(coup_model);
}

void SimEngine::ScheduleDevAtomic(const Time& ta, BaseModel* src_model) {
	if (ta < 0.0) {
		ELOG->critical(
			"The reach_time advance value should be 0 or a positive value, ta = {}",
			ta);
		throw std::runtime_error("Negative ta in SimEngine::ScheduleIntEvent");
	}

	const auto tn = cur_cycle_ + ta;

	ELOG->debug("   - sch.: {} at tn = {}", src_model->GetName(), tn);

	//Record the scheduled event
	BaseEv* ptr_ev = new ModelSchEv(tn, src_model);

	// add the event
	priority_queue_.Add(ptr_ev);

	//Record the scheduled event
	dynamic_cast<DevsAtomic*>(src_model)->SetLastSchEv(ptr_ev);
}

BaseEv* SimEngine::SendData(BaseModel* src_model, const PortId& out_port, std::shared_ptr<void>&& ptr_msg_value,
                            const int& priority, const Time& delay) {
	if (delay < 0) {
		ELOG->critical("Negative delay = {}", delay);
		throw std::runtime_error("Error in SimEngine::SendData");
	}

	if (!src_model->HasOutPort(out_port)) {
		ELOG->critical("Src. ({}) has not out_port, {},{}", src_model->GetName(), out_port,
		               src_model->GetOutPortName(out_port));
		throw std::runtime_error("Error in SimEngine::SendData");
	}

	const auto event_time = cur_cycle_ + delay;
	auto* const ret_ev
		= new ExtEv(event_time, src_model, out_port,
		            nullptr, kUnknownPort, priority, ptr_msg_value);
	priority_queue_.Add(ret_ev);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
	ELOG->debug("   (y) sch. {}: {}/{}, at {}, ev={}",
	            src_model->GetName(), out_port, src_model->GetOutPortName(out_port),
	            cur_cycle_, GetStringFromMsg(ret_ev)
	);
#endif

	return ret_ev;
}

BaseEv* SimEngine::ScheduleEvent(ExtEv* ext_ev) {
	priority_queue_.Add(ext_ev);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
	ELOG->debug("   (y) sch. {}, at {}, ev={}",
	            ext_ev->GetSrcPortName(), cur_cycle_, GetStringFromMsg(ext_ev)
	);
#endif
	return ext_ev;
}

const Time& SimEngine::GetCurClock() const {
	return cur_cycle_;
}

void SimEngine::SetCurClock(const Time& cur_clock) {
	cur_cycle_ = cur_clock;
}

BaseEv* SimEngine::MakeEvent(BaseModel* src_model, BaseModel* dest_model,
                             const PortId& in_port, std::shared_ptr<void>&& value_ptr,
                             const int& priority, const Time& delay) const {

	BaseEv* ret_ev = nullptr;

	std::string src_name("null");
	if (src_model != nullptr) {
		src_name = src_model->GetName();
	}

	if (delay < 0) {
		ELOG->critical("   (y) send: {}:{} => {} with delay({})", src_name, in_port,
		               dest_model->GetName(), delay);
		throw std::runtime_error("Negative delay in SimEngine::SendData");
	}

	if (const auto deliver_time = cur_cycle_ + delay; dest_model != nullptr) {
		if (!dest_model->HasInPort(in_port)) {

			MLOG->critical("{}'s wrong input port: {}",
			               dest_model->GetName(),
			               dest_model->GetInPortName(in_port));

			dest_model->ShowIoSummary();

			throw std::runtime_error(
				fmt::format("{}'s unknown in_port: {}", dest_model->GetName(), in_port));
		}

		ret_ev = new ExtEv(deliver_time, src_model, kUnknownPort,
		                   dest_model, in_port, priority, value_ptr);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
		ELOG->debug("   (y) send: {}:{} => {}, p={}, at={}, ev={}",
		            src_name, in_port, dest_model->GetName(), priority,
		            cur_cycle_, GetStringFromMsg(ret_ev)
		);
#endif
	}
	else if (in_port == static_cast<PortId>(EvType::COUP_CHANGE_TYPE)) {
		ret_ev = new CoupChEv(deliver_time, src_model, priority, value_ptr);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
		ELOG->debug("   (y) send: {} => {} ({}), at={}, ev={}",
		            GetModelName(src_model), "Eng", "COUP_CHANGE",
		            cur_cycle_,
		            GetStringFromMsg(ret_ev));
#endif
	}
	else {
		ELOG->critical("null destination");
	}

	return ret_ev;
}

BaseEv* SimEngine::SendData(BaseModel* src_model, BaseModel* dest_model,
                            const PortId& in_port, std::shared_ptr<void>&& value_ptr,
                            const Time& delay) {

	return SendData(src_model, dest_model, src_model->GetPriority(), in_port, std::move(value_ptr), delay);
}

BaseEv* SimEngine::SendData(BaseModel* src_model, BaseModel* dest_model, const int& priority, const PortId& in_port,
                            std::shared_ptr<void>&& value_ptr, const Time& delay) {

	auto* const ret_ev = MakeEvent(src_model, dest_model, in_port,
	                               std::move(value_ptr), priority, delay);
	if (ret_ev == nullptr) { throw std::runtime_error("invalid event pointer"); }
	priority_queue_.Add(ret_ev);
	return ret_ev;
}

bool SimEngine::Start() {

	//generate initial int events
	try {
		ELOG->debug("** Engine Starts ..");

#ifdef EN_EV_COUNT
		num_ev_ = 0;
#endif

		cur_cycle_ = 0;
		end_cycle_ = kInfinity;

		// Generate initial AM events
		InitAtomicModels(initial_model_.get());
	}
	catch (std::runtime_error& e) {
		ELOG->critical("** Sim failed: {}", e.what());
		CLOG->critical("** Sim failed: {}", e.what());
		return false;
	}
	catch (...) {
		CLOG->critical("** Unknown exception during initialization");
		return false;
	}

	// simulation run
	return run();
}

bool SimEngine::run() {
	std::unique_ptr<BaseEv> ev_sm_ptr;
#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
	ELOG->debug("T = {}", cur_cycle_);
#endif
	try {
		for (auto next_cycle = 0;;) {
			if (priority_queue_.IsEmpty()) {
				// no event exists?
				break;
				// next_cycle = kInfinity;
			}

			/* Get minimum reach_time of scheduled events */
			next_cycle = priority_queue_.GetMinEvTime();

			/* check the simulation-end condition */
			if (next_cycle > end_cycle_) {
				cur_cycle_ = next_cycle;
				break;
			}

			/* update the new reach_time */
			if (next_cycle > cur_cycle_) {
				cur_cycle_ = next_cycle;
#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
				ELOG->debug("");
				ELOG->debug("T = {}", cur_cycle_);
#endif
			}

			/* Pop the front event */
			ev_sm_ptr.reset(priority_queue_.GetFrontEvAfterPop());
			if (ev_sm_ptr->IsActive()) {
#ifdef EN_EV_COUNT
				++num_ev_;
#endif
				const auto& ev_type = ev_sm_ptr->GetEvType();

				if (ev_type == EvType::INT_EV_TYPE) {
					if (!HandleIntEvent(ev_sm_ptr.get())) {
						throw std::runtime_error(
							"False return in HandleIntEvent");
					}
				}
				else if (ev_type == EvType::EXT_EV_TYPE) {
					if (ev_sm_ptr->dest_model_ == nullptr) {
						const auto& dest_model_port_pairs = ev_sm_ptr->GetDestPortList();
						if (dest_model_port_pairs.empty()) {
							throw std::runtime_error(
								fmt::format("missed out_port: {}", ev_sm_ptr->GetSrcPortName()));
						}

						for (const auto& [dest_model, in_port] : dest_model_port_pairs) {
							ev_sm_ptr->SetDestInPort(dest_model, in_port);
							if (!HandleExtEvent(ev_sm_ptr.get())) {
								throw std::runtime_error("False return in HandleExtEvent");
							}
						}
					}
					else {
						if (!HandleExtEvent(ev_sm_ptr.get())) {
							throw std::runtime_error("False return in HandleExtEvent");
						}
					}
				}
				else if (ev_type == EvType::COUP_CHANGE_TYPE) {
					if (!HandleCoupChEvent(ev_sm_ptr.get())) {
						throw std::runtime_error("False return in HandleCoupChEvent");
					}
				}
				else {
					throw std::runtime_error("Unknown ev_type in Run");
				}
			}
			else {
#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
				ELOG->debug("skip: {}", GetStringFromMsg(ev_sm_ptr.get()));
#endif
			}
		}
	}
	catch (std::runtime_error& e) {
		ELOG->critical("Error: {}, at c = {}", e.what(), cur_cycle_);
		ELOG->critical("** Simulation unsuccessfully ends ...");
		return false;
	}
	catch (...) {
		ELOG->critical("Unknown Exception at c = {}", cur_cycle_);
		ELOG->critical("** Simulation unsuccessfully ends ...");
		return false;
	}

	ELOG->debug("** Simulation successfully ends ...");
	return true;
}

void SimEngine::InitAtomicModels(CoupModel* coup_model) {
	auto& child_vec = coup_model->child_models_;

	for (auto* each_child : child_vec) {
		if (each_child->GetType() == kCoupled) {
			InitAtomicModels(dynamic_cast<CoupModel*>(each_child));
		}
		else if (each_child->GetType() == kDevsAtomic) {
			auto ta = dynamic_cast<DevsAtomic*>(each_child)->TimeAdvanceFn();

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
			ELOG->debug("DEVS(i,0) from {}. ta = {}",
			            each_child->GetName(), ta);
#endif

			if (ta != kInfinity) {
				ScheduleDevAtomic(ta, each_child);
			}
		}
		else if (each_child->GetType() == kEoAtomic) {
			Message out_msg(0, each_child);

			if (!dynamic_cast<EoModel*>(each_child)->Initialize(out_msg)) {
				ELOG->critical("Initialize of EO Model = {}, returning failed with msg = {}",
				               each_child->GetName(), out_msg.ToString());
				throw std::runtime_error("EO Initialization goes failed");
			}

			DeliverOutMessages(each_child, out_msg);
		}
	}
}

void SimEngine::ConvertToMessage(Message& conv_in_msg, ExtEv* ext_ev) const {
	auto* src_model = ext_ev->GetSrcModel();

	conv_in_msg = Message(cur_cycle_, src_model);
	conv_in_msg.SetPortValue(ext_ev->GetDestInPort(), ext_ev->GetMsg());

	auto* dest_model = ext_ev->GetDestModel();
	const auto src_out_port = ext_ev->GetSrcOutPort();
	const auto dest_in_port = ext_ev->GetDestInPort();

	std::string out_port_str("unknown");
	if (src_model) {
		out_port_str = src_model->GetOutPortName(src_out_port);
	}

	const auto [_src_out_port, _src_out_port_str] = GetPortIdStrPair(src_out_port, out_port_str);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
	std::string src_name("null");
	if (src_model) { src_name = src_model->GetName(); }

	ELOG->debug(" * {} ({},{}) <= {} ({},{}) {}",
	            dest_model->GetName(), dest_in_port,
	            dest_model->GetInPortName(dest_in_port),
	            src_name, _src_out_port,
	            _src_out_port_str, GetStringFromMsg(ext_ev)
	);
#endif
}

void SimEngine::ConvertToMessage(Message& conv_in_msg, CoupChEv* coup_ch_ev) const {
	auto* const src_model = coup_ch_ev->GetSrcModel();

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
	std::string src_name("null");
	if (src_model) { src_name = src_model->GetName(); }
	ELOG->debug(" * ENG <= {} ({}), {}", src_name, "COUP_CHANGE_EV", GetStringFromMsg(coup_ch_ev));
#endif

	conv_in_msg = Message(cur_cycle_, src_model);
	conv_in_msg.SetPortValue(0, coup_ch_ev->GetMsg());
}

void SimEngine::DeliverOutMessages(BaseModel* src_model, Message& out_msg) {
	// process output message
	std::deque<std::pair<PortValue, Time>> out_msg_deque;
	out_msg.ExtractPortValues(out_msg_deque);
	const auto priority =
		out_msg.IsPriorityValid() ? out_msg.Priority() : src_model->GetPriority();

	// for each src_port and value pair
	for (auto& [port_value_pair, delay] : out_msg_deque) {
		auto& [out_port, value] = port_value_pair;
		SendData(src_model, out_port, std::move(value), priority, delay);
	}
}

bool SimEngine::HandleIntEvent(BaseEv* ptr_ev) {
	auto* const dest_model = ptr_ev->GetDestModel();

	if (dest_model->GetType() == kDevsAtomic) {
		auto* dest_devs_atomic = dynamic_cast<DevsAtomic*>(dest_model);

#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
		ELOG->debug(" * {} <= DEVS(*,{})", dest_devs_atomic->GetName(), cur_cycle_);
#endif

		// execute output function
		Message out_msg(cur_cycle_, ptr_ev->GetDestModel());
		if (!dest_devs_atomic->OutputFn(out_msg)) {
			ELOG->critical("outputFn of DEVS AM ({}) return failed at t = {}",
			               dest_devs_atomic->GetName(), cur_cycle_);
			throw std::runtime_error("outputFn return false");
		}

		DeliverOutMessages(dest_model, out_msg);

		if (!dest_devs_atomic->IntTransFn()) {
			ELOG->critical("intTransFn of DEVS AM ({}) return failed at t = {}",
			               dest_devs_atomic->GetName(),
			               cur_cycle_);
			throw std::runtime_error("intTransFn return false");
		}

		const auto ta = dest_devs_atomic->TimeAdvanceFn();

		if (ta != kInfinity) {
			ScheduleDevAtomic(ta, dest_devs_atomic);
		}
		else {
			dest_devs_atomic->SetLastSchEv(nullptr);
		}
	}
	else {
		return false;
	}

	return true;
}

bool SimEngine::HandleExtEvent(BaseEv* ptr_ev) {
	auto* ext_ev = dynamic_cast<ExtEv*>(ptr_ev);
	auto* const dest_model = ext_ev->GetDestModel();

	if (dest_model->GetType() == kDevsAtomic) {
		auto* const dest_devs_atomic = dynamic_cast<DevsAtomic*>(dest_model);

		Message msg;
		ConvertToMessage(msg, ext_ev);

		if (!dest_devs_atomic->ExtTransFn(msg)) {
			ELOG->critical("ExtTransFn of DEVS AM ({}) return failed with msg (={})",
			               dest_devs_atomic->GetName(), msg.ToString());
			throw std::runtime_error("DEVS ExtTransFn goes failed");
		}

		if (!dest_devs_atomic->GetContinueValue()) {
			//Nullify the previous int sch. event
			if (dest_devs_atomic->GetLastSchEv() != nullptr) {
				auto* prev_sch_ev = dest_devs_atomic->GetLastSchEv();
				prev_sch_ev->Nullify();

				ELOG->debug(" - Null => sch. event ({}, tn = {})",
				            dest_devs_atomic->GetName(), prev_sch_ev->GetTime());

				//Nullify the previous model schedule event
				dest_devs_atomic->SetLastSchEv(nullptr);
			}

			if (const auto ta = dest_devs_atomic->TimeAdvanceFn();
				ta != kInfinity)
				ScheduleDevAtomic(ta, dest_devs_atomic);
		}
		else {
#if !defined(NDEBUG) || defined (ENG_LOG_TRACE_ENABLE)
			ELOG->debug(" - Sch. reach_time is not changed, sch. event ({}, tn = {})",
			            dest_devs_atomic->GetName(),
			            dest_devs_atomic->GetLastSchEv()->GetTime());
#endif
		}
	}
	else if (dest_model->GetType() == kEoAtomic) {
		auto* eo_model = dynamic_cast<EoModel*>(dest_model);

		Message in_msg;
		ConvertToMessage(in_msg, ext_ev);
		Message out_msg(cur_cycle_, ptr_ev->GetDestModel());

		if (!eo_model->TransFn(in_msg, out_msg)) {
			ELOG->critical("The transFn of EO Model ({}) returns failed, msg (={})",
			               eo_model->GetName(), out_msg.ToString());
			throw std::runtime_error("EO TransFn goes failed");
		}

		DeliverOutMessages(eo_model, out_msg);
	}
	else {
		return false;
	}

	return true;
}

bool SimEngine::HandleCoupChEvent(BaseEv* ptr_ev) const {
	auto* coup_ch_ev = dynamic_cast<CoupChEv*>(ptr_ev);
	Message in_msg;
	ConvertToMessage(in_msg, coup_ch_ev);

	if (dyn_coup_handler_) {
		return dyn_coup_handler_->HandleEvent(in_msg);
	}

	throw std::runtime_error(
		"Dynamic coupling handler is not registered");

	return false;
}

void SimEngine::SetDynCoupHandler(BaseDynCoupHandler* coup_handler) {
	dyn_coup_handler_.reset(coup_handler);
}

BaseDynCoupHandler* SimEngine::GetDynCoupHandler() const {
	if (dyn_coup_handler_) {
		return dyn_coup_handler_.get();
	}

	return nullptr;
}

void SimEngine::ShowModelInfo() const {
	if (initial_model_ == nullptr) {
		throw std::runtime_error("a model need to be assigned before ShowModInfo()");
	}

	ELOG->info("** [Info Start]");
	ShowModelInfo(initial_model_.get());

	if (dyn_coup_handler_) {
		dyn_coup_handler_->PrintDynCoupling();
	}

	ELOG->info("** [Info End]");
}

void SimEngine::ShowModelInfo(CoupModel* coup_model) const {
	//print current coupled model info

	coup_model->ShowIoSummary();
	auto& child_vec = coup_model->child_models_;

	for (auto* each_child : child_vec) {
		if (each_child->GetType() == kCoupled) {
			ShowModelInfo(dynamic_cast<CoupModel*>(each_child));
		}
		else {
			each_child->ShowIoSummary();
			each_child->ShowDestSummary();
		}
	}
}

std::pair<std::string, std::string>
SimEngine::GetPortIdStrPair(const PortId& out_port_id, const std::string& out_port_str) const {
	if (out_port_id == kUnknownPort) {
		return { "", "Unknown" };
	}

	return { std::to_string(out_port_id), out_port_str };
}

SimEngine::SimEngine() = default;

SimEngine::~SimEngine() {
	Reset();
}
