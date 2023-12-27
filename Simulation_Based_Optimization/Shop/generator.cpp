#include "generator.h"
#include <numeric>
#include "logger.h"
#include "agv.h"

Generator::Generator(std::deque<int> job_seq) : waiting_jobs_(std::move(job_seq)) {
	assert(!waiting_jobs_.empty());
	SetName(GetCellString(CellType::kInv));
	AddInPort(0, "ArrFetch");
	AddInPort(1, "Avail");

	AddOutPort(0, "ReqToAgv");
	AddOutPort(1, "ReqCancel");

	// register ev. handlers
	next_avail_map_ = { { CellType::kCell1, true }, };

	RegisterEvHandler(0, [this](const auto& in_msg, auto& out_msg) {
		return Generator::HandleArrFetch(in_msg, out_msg);
	});

	RegisterEvHandler(1, [this](const auto& in_msg, auto& out_msg) {
		return Generator::HandleAvail(in_msg, out_msg);
	});

}

Generator::~Generator() = default;

bool Generator::Initialize(Message& out_msg) {
	assert(!waiting_jobs_.empty());
	out_msg.SetPortValue(GetOutPortId("ReqToAgv"),
	                     new std::pair<Time, CellType>(cur_clock_, cur_cell_));

	return true;
}

bool Generator::HandleAvail(const Message& in_msg, Message& out_msg) {
	const auto& [next_cell, is_avail] =
		*static_cast<std::pair<CellType, bool>*>(in_msg.Value());
	next_avail_map_.at(next_cell) = is_avail;

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}: {}.avail <= {}", name_, GetCellString(next_cell), is_avail);
#endif

	if (is_avail && !waiting_jobs_.empty()) {
		ReqFetchToAgv(out_msg);
	}

	return true;
}

bool Generator::HandleArrFetch(const Message& in_msg, Message& out_msg) {
	// get part type
	const auto part_type = waiting_jobs_.front();
	waiting_jobs_.pop_front();

	dynamic_cast<Agv*>(in_msg.GetSrcModel())->
		StartDeliveryToNext(part_type, cur_cell_, CellType::kCell1);

	return true;
}

void Generator::ReqFetchToAgv(Message& out_msg) {
	out_msg.SetPortValue(GetOutPortId("ReqToAgv"), new std::pair(cur_clock_, cur_cell_));
}
