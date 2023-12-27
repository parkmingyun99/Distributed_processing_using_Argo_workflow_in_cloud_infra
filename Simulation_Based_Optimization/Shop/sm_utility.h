#pragma once

#include "sim_type.h"
#include <utility>
#include <string>
#include <vector>
#include <unordered_map>

class Message;

/* Model priority */
/* lower number is higher */
constexpr int kPriCell = 50;
constexpr int kPriAgv = 200;
constexpr int kMaxPriority = std::numeric_limits<int>::max();;

enum class CellType {
	kInv = 0,
	kCell1 = 1,
	kCell2_1 = 2,
	kCell2_2 = 3,
	kCell3 = 4,
	kTrans = 5,
	kCellMax = 6,
};

using Loc = int;
static std::unordered_map<CellType, Loc> glo_loc_map = {
	{ CellType::kInv, 0 },
	{ CellType::kCell1, 1 },
	{ CellType::kCell2_1, 2 },
	{ CellType::kCell2_2, 2 },
	{ CellType::kCell3, 3 },
	{ CellType::kTrans, 4 },
};

constexpr int kCellDelay = 20;
Time GetMoveDelay(const CellType& lhs, const CellType& rhs);
std::string GetCellString(const CellType& cell_type);
using FetchFeqMsgType = std::pair<Time, CellType>; // fetch time, cell, is_add

struct Delivery {
	Time fetch_avail_time{};
	CellType req_cell;

	Delivery(const Time& proc_end_time, const CellType& req_cell);
	std::pair<Time, Time> GetMoveStartArrTimePair(const Time& cur_clock, const Time& move_time);
	[[nodiscard]] std::string GetString() const;
};

