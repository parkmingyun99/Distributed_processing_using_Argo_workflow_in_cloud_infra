#pragma once

#include "lib_sm_model.h"
#include "eo_model.h"
#include "sm_utility.h"

class SM_MODEL_CLASS Generator final : public EoModel {
public:
    explicit Generator(std::deque<int> job_seq);

    ~Generator() override;

    // default constructors and operators
    Generator(const Generator &) = delete;

    Generator &operator=(const Generator &) = delete;

    Generator(const Generator &&) = delete;

    Generator &operator=(const Generator &&) = delete;

    bool Initialize(Message &out_msg) override;

    bool HandleAvail(const Message &in_msg, Message &out_msg);

    bool HandleArrFetch(const Message &in_msg, Message &out_msg);

protected:
    void ReqFetchToAgv(Message &out_msg);

    std::deque<int> waiting_jobs_{};
    const CellType cur_cell_{CellType::kInv};
    std::unordered_map<CellType, bool> next_avail_map_;
};
