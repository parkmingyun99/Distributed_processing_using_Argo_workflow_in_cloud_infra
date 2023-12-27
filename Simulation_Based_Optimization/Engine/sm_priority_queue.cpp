#include "sm_priority_queue.h"

void SmPriorityQueue::Add(BaseEv* ptr_ev) {
	// std::lock_guard<std::mutex> lock(priority_queue_lock_);
	priority_queue_.push(ptr_ev);
	ev_set_.insert(ptr_ev);
}

bool SmPriorityQueue::IsEmpty() const {
	return priority_queue_.empty();
}

Time SmPriorityQueue::GetMinEvTime() const {
	return priority_queue_.top()->GetTime();
}

BaseEv* SmPriorityQueue::GetFrontEvAfterPop() {
	auto* const ret_pt_ev = priority_queue_.top();
	// Erase the pointer
	priority_queue_.pop();
	ev_set_.erase(ret_pt_ev);

	return ret_pt_ev;
}

void SmPriorityQueue::GetPrevGenEvents(std::vector<BaseEv*>& prev_gen_events,
                                       BaseModel* source) {
	prev_gen_events.clear();

	for (const auto& each_ev : ev_set_) {
		if (each_ev->src_model_ == source) {
			prev_gen_events.push_back(each_ev);
		}
	}
}

void SmPriorityQueue::NullifyPrevEvents(BaseModel* source) {
	for (const auto& each_ev : ev_set_) {
		if (each_ev->src_model_ == source) {
			each_ev->Nullify();
		}
	}
}

void SmPriorityQueue::clear() {
	// delete each event
	while (!priority_queue_.empty()) {
		delete priority_queue_.top();
		priority_queue_.pop();
	}

	ev_set_.clear();
}
