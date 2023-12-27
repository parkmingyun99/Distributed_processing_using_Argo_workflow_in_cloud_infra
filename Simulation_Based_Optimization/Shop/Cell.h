#pragma once

#include "lib_sm_model.h"
#include "eo_model.h"
#include "sm_utility.h"
#include <queue>

class Cell final : public EoModel {
public:
	explicit Cell(const CellType& cell_type, const int& out_port_num);

	~Cell() override;

	// default constructors and operators
	Cell(const Cell&) = delete;

	Cell& operator=(const Cell&) = delete;

	Cell(const Cell&&) = delete;

	Cell& operator=(const Cell&&) = delete;

	bool HandleAgvDelivery(const Message& in_msg, Message& out_msg);

	bool HandleAvail(const Message& in_msg, Message& out_msg);

	bool HandleAgvFetch(const Message& in_msg, Message& out_msg);

protected:
	[[nodiscard]] CellType GetNextAvailCell() const;
	const CellType cur_cell_;

	std::unordered_map<CellType, bool> next_avail_map_;
	std::unordered_map<int, Time> proc_time_map_;

	// proc. info
	int proc_part_type_{ -1 };
	Time proc_end_time_{ -1 };
	CellType dep_cell_{ CellType::kCellMax };

	void ReqFetchToAgv(Message& out_msg, const Time& proc_end_time, const CellType& next_avail_cell);
};
