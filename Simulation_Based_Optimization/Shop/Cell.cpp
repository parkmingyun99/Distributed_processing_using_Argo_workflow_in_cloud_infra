//
// Created by smgs2 on 10/17/2022.
//

#include "Cell.h"
#include "logger.h"
#include "agv.h"
#include "csv_export.h"

Cell::Cell(const CellType& cell_type, const int& out_port_num) : cur_cell_(cell_type) {
	/* Make Inputs ports */
	SetName(GetCellString(cell_type));
	SetPriority(kPriCell);

	AddInPort(0, "ArrFetch");
	AddInPort(1, "ArrDep");
	AddInPort(2, "Avail");

	AddOutPort(0, "Avail");
	AddOutPort(1, "ReqToAgv");
	AddOutPort(2, "ReqCancel");

	RegisterEvHandler(0, [this](const auto& in_msg,
	                            auto& out_msg) {
		return Cell::HandleAgvFetch(in_msg, out_msg);
	});

	RegisterEvHandler(1, [this](const auto& in_msg,
	                            auto& out_msg) {
		return Cell::HandleAgvDelivery(in_msg, out_msg);
	});

	RegisterEvHandler(2, [this](const auto& in_msg,
	                            auto& out_msg) {
		return Cell::HandleAvail(in_msg, out_msg);
	});

	// setting proc_time_map and out_map
	if (cur_cell_ == CellType::kCell1) {
		proc_time_map_ = {
			{ 1, 325 },
			{ 2, 255 },
			{ 3, 315 },
			{ 4, 415 },
		};
		/**/
		next_avail_map_ = {
			{ CellType::kCell2_1, true },
			{ CellType::kCell2_2, true },
		};
	}
	else if (cur_cell_ == CellType::kCell2_1 || cur_cell_ == CellType::kCell2_2) {
		proc_time_map_ = {
			{ 1, 400 },
			{ 2, 310 },
			{ 3, 255 },
			{ 4, 340 },
		};
		next_avail_map_ = {
			{ CellType::kCell3, true },
		};
	}
	else if (cur_cell_ == CellType::kCell3) {
		proc_time_map_ = {
			{ 1, 375 },
			{ 2, 355 },
			{ 3, 410 },
			{ 4, 310 },
		};
		next_avail_map_ = {
			{ CellType::kTrans, true },
		};
	}
	else {
		throw std::runtime_error("unknown cell type");
	}
}

Cell::~Cell() = default;

bool Cell::HandleAgvDelivery(const Message& in_msg, Message& out_msg) {
	// inform agv of delivery ending
	dynamic_cast<Agv*>(in_msg.GetSrcModel())->SetDeliveryEnd(cur_cell_);
	out_msg.SetPortValue(GetOutPortId("Avail"), new std::pair(cur_cell_, false));

	// Get new part and its processing time
	proc_part_type_ = *static_cast<int*>(in_msg.Value());
	const auto proc_time = proc_time_map_.at(proc_part_type_);

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	proc_end_time_ = cur_clock_ + proc_time;
	MLOG->trace(" @ {}.process 'A{}' with proc/end time = '{}/{}'", name_, proc_part_type_,
	            proc_time, proc_end_time_);
#endif

	CsvExport::Instance().RegisterTask({
		name_,
		cur_clock_,
		proc_end_time_,
		fmt::format("A{}", proc_part_type_),
		{}
	});

	// req fetch
	if (dep_cell_ != CellType::kCellMax) {
		throw std::runtime_error("dep_cell is already allocated");
	}
	const auto next_avail_cell = GetNextAvailCell();
	if (next_avail_cell != CellType::kCellMax) {
		ReqFetchToAgv(out_msg, proc_end_time_, next_avail_cell);
	}

	return true;
}

bool Cell::HandleAvail(const Message& in_msg, Message& out_msg) {
	if (cur_clock_ == 1480) {
		int a = 3;
	}
	const auto& [next_cell, is_avail] = *static_cast<std::pair<CellType, bool>*>(in_msg.Value());
	next_avail_map_.at(next_cell) = is_avail;

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
	MLOG->trace(" @ {}: {}.avail <= {}", name_, GetCellString(next_cell), is_avail);
#endif

	if (is_avail) {
		if (proc_part_type_ >= 0 && dep_cell_ == CellType::kCellMax) {
			const auto fetch_time = std::max(proc_end_time_, cur_clock_);
			ReqFetchToAgv(out_msg, fetch_time, next_cell);
		}
	}
	else if (proc_part_type_ >= 0) {
		dep_cell_ = CellType::kCellMax;
		out_msg.SetPortValue(GetOutPortId("ReqCancel"), new auto(cur_cell_));
	}
	return true;
}

bool Cell::HandleAgvFetch(const Message& in_msg, Message& out_msg) {
	assert(proc_part_type_ >= 0);

	dynamic_cast<Agv*>(in_msg.GetSrcModel())->
		StartDeliveryToNext(proc_part_type_, cur_cell_, dep_cell_);

	// initialize cur_proc
	proc_part_type_ = -1;
	dep_cell_ = CellType::kCellMax;

	// notify avail to prev. cell
	out_msg.SetPortValue(GetOutPortId("Avail"), new std::pair(cur_cell_, true));

	return true;

}

CellType Cell::GetNextAvailCell() const {
	for (const auto& [each_avail, avail] : next_avail_map_) {
		if (avail) {
			return each_avail;
		}
	}

	return CellType::kCellMax;
}

void Cell::ReqFetchToAgv(Message& out_msg, const Time& proc_end_time, const CellType& next_avail_cell) {
	dep_cell_ = next_avail_cell;
	out_msg.SetPortValue(GetOutPortId("ReqToAgv"), new FetchFeqMsgType(proc_end_time, cur_cell_));
}
