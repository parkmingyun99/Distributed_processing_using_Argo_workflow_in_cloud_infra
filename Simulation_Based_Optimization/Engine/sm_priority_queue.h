#pragma once
#include <queue>
#include <unordered_set>
#include "base_ev.h"

struct EventComparator {
	bool operator()(const BaseEv* ev1, const BaseEv* ev2) const {
		if (ev1->time_ > ev2->time_) return true;
		if (ev1->time_ == ev2->time_) {
			if (ev1->GetEvType() > ev2->GetEvType()) {
				return true;
			}
			if (ev1->GetEvType() == ev2->GetEvType()) {
				if (ev1->priority_ > ev2->priority_) {
					return true;
				}
			}
		}

		return false;
	}
};

template <class T, class Comp>
struct StablePriorityQueue {
	using CounterType = std::size_t;

	struct Proxy {
		Proxy(T&& o, const CounterType c)
			: object(std::move(o))
			  , insertion_order(c) {
		}

		Proxy(const T& o, const CounterType c)
			: object(o)
			  , insertion_order(c) {
		}

		T object;
		CounterType insertion_order;
	};

	struct ProxyComp {
		bool operator ()(const Proxy& l, const Proxy& r) const {
			if (major_order(l.object, r.object))
				return true;
			if (major_order(r.object, l.object))
				return false;
			return minor_order(l.insertion_order, r.insertion_order);
		}

		Comp major_order;
		std::greater<> minor_order;
	};


	decltype(auto) push(T item) {
		return queue.emplace(std::move(item), counter++);
	}

	const T& top() const {
		return queue.top().object;
	}

	void pop() {
		queue.pop();
		if (queue.empty())
			counter = 0;
	}

	[[nodiscard]] bool empty() const {
		return counter == 0;
	}

	std::priority_queue<Proxy, std::vector<Proxy>, ProxyComp> queue;
	CounterType counter = 0;
};

class SmPriorityQueue {
public:
	void Add(BaseEv* ptr_ev);
	[[nodiscard]] bool IsEmpty() const;
	[[nodiscard]] Time GetMinEvTime() const; // get minimum wait_time of scheduled internal events
	BaseEv* GetFrontEvAfterPop();
	void GetPrevGenEvents(std::vector<BaseEv*>& prev_gen_events, BaseModel* source);
	void NullifyPrevEvents(BaseModel* source);

	void clear();


	// std::priority_queue<BaseEv*, std::vector<BaseEv*>,
	//                     EventComparator> priority_queue_{};
	StablePriorityQueue<BaseEv*, EventComparator> priority_queue_;
	std::unordered_set<BaseEv*> ev_set_;
};
