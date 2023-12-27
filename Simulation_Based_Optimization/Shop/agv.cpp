//
// Created by smgs2 on 10/16/2022.
//

#include "agv.h"
#include <cassert>

#include "csv_export.h"
#include "logger.h"
#include "sm_utility.h"

void DeliveryWaitingQueue::Add(const Delivery& delivery_req) {
	const auto& req_cell = delivery_req.req_cell;
	assert(!req_map.contains(req_cell));
	req_map.emplace(req_cell, delivery_req);
}

NextReq DeliveryWaitingQueue::Pop(const Time& cur_clock, const CellType& cur_cell) {

	if (req_map.empty()) {
		return { -1, -1, CellType::kCellMax };
	}

	Time min_mst{ kInfinity }, min_rt{ kInfinity };
	std::vector<FetchMove> fetch_vec;

	for (auto& [req_cell, each_req] : req_map) {
		const auto move_time = GetMoveDelay(req_cell, cur_cell);
		const auto [each_mst, each_rt] =
			each_req.GetMoveStartArrTimePair(cur_clock, move_time);

		if (each_mst < min_mst) min_mst = each_mst;
		if (each_rt < min_rt) min_rt = each_rt;

		fetch_vec.emplace_back(req_cell, each_mst, each_rt, move_time);

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace("    -  {}.req : {}, {}, {} / min_mst = {}, min_rt = {}", GetCellString(req_cell), each_mst, each_rt, move_time ,min_mst,min_rt);
#endif

	}

	std::pair overhead_index_pair{ kInfinity, -1 };
	auto index = 0;
	for (const auto& [req_cell, each_st, each_rt, move_time]
	     : fetch_vec) {
		const auto each_overhead = (each_st - min_mst) + (each_rt - min_rt);

		if (const auto& min_overhead = overhead_index_pair.first;
			each_overhead <= min_overhead) {
			overhead_index_pair = { each_overhead, index };
		}

		++index;
	}

	const auto& [grant_cell, mst, rt, move_time]
		= fetch_vec.at(overhead_index_pair.second);

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}.grant : {}, {}, {}", GetCellString(grant_cell), mst, rt, move_time);
#endif

	// erase 
	req_map.erase(grant_cell);

	return { mst-cur_clock, move_time, grant_cell  };
}

bool DeliveryWaitingQueue::Empty() const {
	return req_map.empty();
}

void DeliveryWaitingQueue::Cancel(const CellType& req_cell) {
	req_map.erase(req_cell);
}

Agv::Agv() {
	SetName("AGV");
	SetPriority(kPriAgv);
	/* Make Inputs ports */
	AddInPort(0, "ReqFromCell");
	// for seft trigger
	AddInPort(1, "DecideNextReq");
	AddInPort(2, "MoveForFetch");
	/* external req. cancel */
	AddInPort(3, "ReqCancel");

	/* Informing cell to fetch */
	for (auto i = 0; i < static_cast<int>(CellType::kCellMax); ++i) {
		AddOutPort(i, fmt::format("ArrFetch{}", GetCellString(static_cast<CellType>(i))));
	}

	/* Informing cell to deliver parts */
	for (auto i = 0; i < static_cast<int>(CellType::kCellMax); ++i) {
		AddOutPort(10 + i, fmt::format("ArrDep{}", GetCellString(static_cast<CellType>(i))));
	}

	/* RegisterTask ev. handler */
	RegisterEvHandler(0, [this](const auto& in_msg, auto& out_msg) {
		return Agv::HandleDeliveryRequest(in_msg, out_msg);
	});

	RegisterEvHandler(1, [this](const auto& in_msg, auto& out_msg) {
		return Agv::DecideNextReq(in_msg, out_msg);
	});

	RegisterEvHandler(2, [this](const auto& in_msg, auto& out_msg) {
		return Agv::MoveToFetch(in_msg, out_msg);
	});

	RegisterEvHandler(3, [this](const auto& in_msg, auto& out_msg) {
		return Agv::CancelRequest(in_msg, out_msg);
	});
}

Agv::~Agv() = default;

bool Agv::HandleDeliveryRequest(const Message& in_msg, Message& out_msg) {
	if( cur_clock_ == 1045) {
		int a =3;
	}
	const auto& [req_time, req_cell] =
		*static_cast<FetchFeqMsgType*>(in_msg.Value());
	const Delivery req(req_time, req_cell);
	req_queue_.Add(req);

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}: {}.req with fetch.time {}", name_, GetCellString(req_cell), req_time);
#endif

	if (state_ == kIdle) {
		SchSelectNextRequest();
	}

	return true;
}

bool Agv::DecideNextReq(const Message& in_msg, Message& out_msg) {
	if (cur_clock_ == 1065) {
		int a= 3;
	}
	sch_state_decide_ = nullptr;
	assert(!req_queue_.Empty());

	if (const auto& [wait_time, move_time, next_cell]
		= req_queue_.Pop(cur_clock_, cur_cell_); wait_time > 0) {
		// scheduling start_move
		SendData(this, 2, new std::pair(move_time, next_cell), wait_time);
#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
		MLOG->trace(" @ {}.state => {} (from {}) at {}", name_, GetString(kWaitForFetch), GetString(state_),
		            cur_clock_);
#endif
		state_ = kWaitForFetch;
	}
	else if (wait_time == 0) {
		MoveForFetch(out_msg, move_time, next_cell);
	}

	return true;
}

bool Agv::MoveToFetch(const Message& in_msg, Message& out_msg) {
	const auto [move_time, next_cell] =
		*static_cast<std::pair<Time, CellType>*>(in_msg.Value());

	MoveForFetch(out_msg, move_time, next_cell);
	return true;
}

bool Agv::CancelRequest(const Message& in_msg, Message& out_msg) {
	const auto req_cell = *static_cast<CellType*>(in_msg.Value());
#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}: {}.cancel at {}",
	            name_, GetCellString(req_cell), cur_clock_);
#endif

	req_queue_.Cancel(req_cell);
	return true;
}

bool Agv::StartDeliveryToNext(const int& part_type,
                              const CellType& req_cell,
                              const CellType& dest_cell) {
	// update req_cell
	cur_cell_ = req_cell;
	state_ = kMoveForDelivery;

	const auto out_port_id = 10 + static_cast<PortId>(dest_cell);
	SendData(out_port_id, new int(part_type), kCellDelay);

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}.state => '{}', cur_cell = '{}', "
	            "next_ell = '{}' (with part = 'A{}')",
	            name_, GetString(state_),
	            GetCellString(cur_cell_),
	            GetCellString(dest_cell), part_type);
#endif

	CsvExport::Instance().RegisterTask({
		name_,
		cur_clock_,
		cur_clock_ + kCellDelay,
		"Delivery",
		fmt::format("{}->{}", GetCellString(req_cell), GetCellString(dest_cell))
	});

	return true;
}

bool Agv::SetDeliveryEnd(const CellType& dest_cell) {
	cur_cell_ = dest_cell;
	state_ = kIdle;

	if (!req_queue_.Empty()) {
		SchSelectNextRequest();
	}

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}.state => '{}', dep_cell = '{}'",
	            name_, GetString(state_), GetCellString(cur_cell_));
#endif
	return true;
}

void Agv::MoveForFetch(Message& out_msg, const Time& move_time, const CellType& next_cell) {
	const auto out_port_id = static_cast<PortId>(next_cell);
	out_msg.SetPortValue(out_port_id, nullptr, move_time);
	state_ = kMoveForFetch;

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}.state => '{}', next_cell = '{}'", name_,
	            GetString(state_), GetCellString(next_cell));
#endif

	CsvExport::Instance().RegisterTask({
		name_,
		cur_clock_,
		cur_clock_ + move_time,
		"Fetch",
		fmt::format("{}->{}", GetCellString(cur_cell_), GetCellString(next_cell))
	});
}

void Agv::SchSelectNextRequest() {
	if (!sch_state_decide_) {
		sch_state_decide_ =
			SendData(this, 1, nullptr);
	}
}

std::string Agv::GetString(const AgvState& state) {
	switch (state) {
	case kIdle: return "IDLE";
	case kWaitForFetch: return "WaitForFetch";
	case kMoveForFetch: return "MoveFoFetch";
	case kMoveForDelivery: return "MoveForDelivery";
	default: return "Unknown";
	}
}
