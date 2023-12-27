#pragma once

#include "lib_sm_model.h"
#include "eo_model.h"
#include "sm_utility.h"
#include <queue>


// move_time, move_time, req_cell
struct NextReq {
	Time wait_time{-1};
	Time move_time{-1};
	CellType next_cell{CellType::kCellMax};
};

struct FetchMove {
	CellType req_cell;
	Time move_start_time;
	Time arr_time;
	Time move_time;
};

struct DeliveryWaitingQueue {
	void Add(const Delivery& delivery_req);
	NextReq Pop(const Time& cur_clock, const CellType& cur_cell);

	void Refresh(const Time& cur_time, const CellType& cur_cell);
	[[nodiscard]] inline bool Empty() const;
	void Cancel(const CellType& req_cell);

	std::map<CellType, Delivery> req_map;
};

class SM_MODEL_CLASS Agv final : public EoModel {
public:
	explicit Agv();

	~Agv() override;

	// default constructors and operators
	Agv(const Agv&) = delete;

	Agv& operator=(const Agv&) = delete;

	Agv(const Agv&&) = delete;

	Agv& operator=(const Agv&&) = delete;

	bool HandleDeliveryRequest(const Message& in_msg, Message& out_msg);
	bool DecideNextReq(const Message& in_msg, Message& out_msg);
	bool MoveToFetch(const Message& in_msg, Message& out_msg);
	bool StartDeliveryToNext(const int& part_type, const CellType& req_cell, const CellType& dest_cell);
	bool SetDeliveryEnd(const CellType& dest_cell);

protected:
	enum AgvState {
		kIdle,
		kWaitForFetch,
		kMoveForFetch,
		kMoveForDelivery
	};

	AgvState state_{ kIdle };
	CellType cur_cell_{ CellType::kInv };

	DeliveryWaitingQueue req_queue_;

	BaseEv* sch_state_decide_{ nullptr };

	void MoveForFetch(Message& out_msg, const Time& move_time,
	                    const CellType& next_cell);
	void SchSelectNextRequest();
	static std::string GetString(const AgvState& state);

	bool CancelRequest(const Message& in_msg, Message& out_msg);
};
