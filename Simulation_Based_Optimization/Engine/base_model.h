#pragma once

#include "lib_engine.h"
#include "sim_type.h"

#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <list>


struct DeleteObj {
	template <typename T>
	void operator()(T* ptr) const {
		delete ptr;
		ptr = nullptr;
	}
};

class SM_SIM_EXT_CLASS BaseModel {
public:
	using DestPortPair = std::pair<BaseModel*, PortId>;

	BaseModel();
	explicit BaseModel(const ModelType& model_type);
	BaseModel(std::string model_name, const ModelType& model_type);
	virtual ~BaseModel() = default;

	void SetName(const std::string& name);
	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] ModelType GetType() const;

	void SetPriority(const int& priority) {
		this->priority_ = priority;
	};
	[[nodiscard]] int GetPriority() const { return priority_; }

	bool AddInPort(const PortId& in_port_id, const std::string& port_name);
	bool AddOutPort(const PortId& out_port_id, const std::string& port_name);

	bool SetDynOutPort(const std::string& flag, const PortId& out_port_id);
	bool SetDynOutPort(const std::string& flag, const std::string& port_name);

	[[nodiscard]] bool HasInPort(const PortId& in_port_id) const;
	[[nodiscard]] bool HasInPort(const std::string& in_port_name) const;

	[[nodiscard]] bool HasOutPort(const PortId& out_port_id) const;
	[[nodiscard]] bool HasOutPort(const std::string& out_port_name) const;
	[[nodiscard]] bool HasDynPort() const;

	void RemoveInPort(const PortId& in_port_id);
	void RemoveOutPort(const PortId& out_port_id);

	std::string GetInPortName(const PortId& port_id);
	PortId GetInPortId(const std::string& in_port_name);

	std::string GetOutPortName(const PortId& port_id);
	PortId GetOutPortId(const std::string& out_port_name);

	void SetParent(BaseModel* model) { parent_model_ = model; }
	[[nodiscard]] inline BaseModel* GetParent() const;

	virtual void ShowIoSummary();
	virtual void ShowDestSummary();

	// clear all port-to-destination information 
	void ResetDestModelPortPairs();
	// Reset the target port's  destinations
	void RemoveDestModelPort(const PortId& out_port, const DestPortPair& dest_port_pair);
	// set coupling
	bool AddDestModelPort(const PortId& out_port, const DestPortPair& dest_port);
	bool ReviseOutPortDest(const PortId& out_port, const DestPortPair& pre_dest_port,
	                       const DestPortPair& new_dest_port);
	// bool AddSavedDestModelPort(const PortId& out_port, const DestPortPair& dest_port);
	std::list<std::pair<BaseModel*, int>>& GetDestModelPortList(const PortId& out_port);

	bool IsDestExist(const PortId& out_port);

	[[nodiscard]] std::string ToInputString() const;
	[[nodiscard]] std::string ToOutputString() const;
	[[nodiscard]] std::string GetDestStr(const PortId& out_port) const;

	//port information
	std::unordered_map<PortId, std::string> in_port_to_name_;
	std::unordered_map<PortId, std::string> out_port_to_name_;
	std::unordered_map<std::string, PortId> in_name_to_port_;
	std::unordered_map<std::string, PortId> out_name_to_port_;

	// for dynamic coupling
	std::unordered_map<std::string, std::set<DestPortPair>> dyn_out_ports_;

private:
	[[nodiscard]] bool IsInPortNameNew(const std::string& port_name) const;
	[[nodiscard]] bool IsOutPortNameNew(const std::string& port_name) const;

protected:
	[[nodiscard]] const Time& GetCurClock() const;
	const Time& cur_clock_;
	std::string name_ = "Undefined";
	ModelType type_ = kUndefModel;
	BaseModel* parent_model_ = nullptr;

	std::unordered_map<PortId, std::list<DestPortPair>> dest_port_pairs_;

	int priority_ = 0;
};

using ModelPortPair = BaseModel::DestPortPair;
