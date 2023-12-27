#include "transducer.h"
#include "logger.h"
#include "agv.h"

Transducer::Transducer(const std::deque<int> &job_seq) {
    SetName(GetCellString(CellType::kTrans));
    SetPriority(kPriCell);

    AddInPort(0, "ArrDep");

    // initialize the received num map
    for (const auto &each_job : job_seq) {
        remaining_jobs_[each_job]++;
    }

    RegisterEvHandler(0, [this](const auto &in_msg,
                                auto &out_msg) {
        return Transducer::HandleArrDep(in_msg, out_msg);
    });
}

Transducer::~Transducer() = default;

bool Transducer::HandleArrDep(const Message &in_msg, Message &out_msg) {
    // inform agv of delivery ending
    dynamic_cast<Agv *>(in_msg.GetSrcModel())->SetDeliveryEnd(cur_cell_);
    // always avail
    //    out_msg.SetPortValue(GetOutPortId("Avail"),
    //                         new std::pair<CellType, bool>(cur_cell_, false));

    // Get new part and its processing time
    auto part_type = *static_cast<int *>(in_msg.Value());
    int remaining_parts = remaining_jobs_.at(part_type);
    remaining_parts--;

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
    MLOG->trace(" @ {}: A{} arr. at {}, remaining = {}", name_,
                part_type, cur_clock_, remaining_parts);
#endif

    if (remaining_parts == 0) {
        remaining_jobs_.erase(part_type);
    }

#if !defined(NDEBUG) || defined(MDL_LOG_TRACE_ENABLE)
    MLOG->trace(" @ {}: job finished at {}", name_, cur_clock_);
#endif
    return true;
}
