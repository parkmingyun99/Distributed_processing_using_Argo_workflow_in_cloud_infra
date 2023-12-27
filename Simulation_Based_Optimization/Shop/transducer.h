#pragma once
#include "lib_sm_model.h"
#include "eo_model.h"
#include "sm_utility.h"

class SM_MODEL_CLASS Transducer final : public EoModel {
public:
    Transducer(const std::deque<int> &job_seq);

    ~Transducer() override;

    // default constructors and operators
    Transducer(const Transducer &) = delete;

    Transducer &operator=(const Transducer &) = delete;

    Transducer(const Transducer &&) = delete;

    Transducer &operator=(const Transducer &&) = delete;

    /*
    void SetGenBound(const std::string& lot_id_header, const int& bound);
    static std::string GetLotHeader(const std::string& lot_id);
    */

    const CellType cur_cell_{CellType::kTrans};
    std::unordered_map<int, int> remaining_jobs_;

    bool HandleArrDep(const Message &in_msg, Message &out_msg);
};
