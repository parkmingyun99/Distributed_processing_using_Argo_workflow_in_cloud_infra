#include "sm_utility.h"
#include "logger.h"

Time GetMoveDelay(const CellType& lhs, const CellType& rhs) {
	auto diff = glo_loc_map.at(lhs) - glo_loc_map.at(rhs);
	if (diff < 0) {
		diff *= -1;
	}

	return diff * kCellDelay;
}

std::string GetCellString(const CellType& cell_type) {
	switch (cell_type) {
	case CellType::kInv: return "INV";
	case CellType::kCell1: return "Cell1";
	case CellType::kCell2_1: return "Cell2-1";
	case CellType::kCell2_2: return "Cell2-2";
	case CellType::kCell3: return "Cell3";
	case CellType::kTrans: return "Trans";
	default:
		return "Unknown";
	}
}

Delivery::Delivery(const Time& proc_end_time,
                   const CellType& req_cell): fetch_avail_time(proc_end_time),
                                              req_cell(req_cell) {
}

std::pair<Time, Time> Delivery::GetMoveStartArrTimePair(const Time& cur_clock, const Time& move_time) {
	if (fetch_avail_time < cur_clock) {
		fetch_avail_time = cur_clock;
	}
	const auto arr_time  = std::max(fetch_avail_time, cur_clock + move_time);
	return {arr_time - move_time, arr_time};
}

