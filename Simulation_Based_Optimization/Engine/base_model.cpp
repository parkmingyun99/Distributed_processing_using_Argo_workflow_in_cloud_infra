#include "base_model.h"
#include "sim_engine.h"

#include <algorithm>
#include <utility>

#include "logger.h"

BaseModel::BaseModel(): cur_clock_(EngInstance().GetCurClock()) {
}

BaseModel::BaseModel(const ModelType& model_type)
	: cur_clock_(EngInstance().GetCurClock()),
	  type_(model_type) {
}

BaseModel::BaseModel(std::string model_name, const ModelType& model_type)
	: cur_clock_(EngInstance().GetCurClock()),
	  name_(std::move(model_name)), type_(model_type) {
}

void BaseModel::SetName(const std::string& name) {
	name_ = name;
}

std::string BaseModel::GetName() const { return name_; }

ModelType BaseModel::GetType() const { return type_; }

bool BaseModel::AddInPort(const PortId& in_port_id, const std::string& port_name) {
	const auto tp = in_port_to_name_.find(in_port_id);

	if (tp == in_port_to_name_.end()) {
		if (IsInPortNameNew(port_name)) {
			in_port_to_name_.emplace(in_port_id, port_name);
			in_name_to_port_.emplace(port_name, in_port_id);
		}
		else {
			ELOG->critical("InPort : src_port name = {} exists in {}", port_name, GetName());
			throw std::runtime_error(fmt::format("{}'s AddInPort {}/{}", name_, in_port_id,
			                                     port_name));
			return false;
		}
	}
	else {
		ELOG->critical("InPort : src_port num = {} exists in {}", in_port_id, GetName());
		throw std::runtime_error(fmt::format("{}'s AddInPort {}/{}", name_, in_port_id, port_name));
		return false;
	}

	return true;
}

bool BaseModel::AddOutPort(const PortId& out_port_id, const std::string& port_name) {
	const auto tp = out_port_to_name_.find(out_port_id);

	if (tp == out_port_to_name_.end()) {
		if (IsOutPortNameNew(port_name)) {
			out_port_to_name_.emplace(out_port_id, port_name);
			out_name_to_port_.emplace(port_name, out_port_id);
		}
		else {
			ELOG->critical("OutPort : src_port name = {} exists in {}", port_name, GetName());
			throw std::runtime_error(fmt::format("{}'s AddInPort {}/{}", name_, out_port_id,
			                                     port_name));
			return false;
		}
	}
	else {
		ELOG->critical("OutPort : src_port num = {} exists in {}", out_port_id, GetName());
		throw std::runtime_error(fmt::format("{}'s AddInPort {}/{}", name_, out_port_id,
		                                     port_name));
		return false;
	}

	return true;
}

bool BaseModel::SetDynOutPort(const std::string& flag, const PortId& out_port_id) {
	if (out_port_to_name_.contains(out_port_id)) {
		dyn_out_ports_[flag].insert({ this, out_port_id });
	}
	else {
		ELOG->critical("SetDynOutPort: out port num = {} cannot be found at {}",
		               out_port_id, GetName());
		throw std::runtime_error(fmt::format("{}'s SetDynOutPort {}/{}", name_, flag, out_port_id));
		return false;
	}

	return true;
}

bool BaseModel::SetDynOutPort(const std::string& flag, const std::string& port_name) {

	for (const auto& each_item : out_port_to_name_) {

		if (each_item.second == port_name) {
			dyn_out_ports_[flag].insert({ this, each_item.first });
			return true;
		}
	}

	ELOG->critical("BaseModel::SetDynOutPort: out port num = {} "
	               "cannot be found at {}", port_name, GetName());
	return false;
}

bool BaseModel::HasInPort(const PortId& in_port_id) const {
	return in_port_to_name_.contains(in_port_id);
}

bool BaseModel::HasInPort(const std::string& in_port_name) const {
	return in_name_to_port_.contains(in_port_name);
}

bool BaseModel::HasOutPort(const PortId& out_port_id) const {
	return out_port_to_name_.contains(out_port_id);
}

bool BaseModel::HasOutPort(const std::string& out_port_name) const {
	return out_name_to_port_.contains(out_port_name);
}

bool BaseModel::HasDynPort() const {
	return !dyn_out_ports_.empty();
}

void BaseModel::RemoveInPort(const PortId& in_port_id) {
	auto tp = in_port_to_name_.find(in_port_id);
	if (tp == in_port_to_name_.end()) {
		ELOG->critical("InPort : {} does not exist in {}", in_port_id, GetName());
	}
	in_port_to_name_.erase(tp);
}

void BaseModel::RemoveOutPort(const PortId& out_port_id) {
	auto tp = out_port_to_name_.find(out_port_id);
	if (tp == out_port_to_name_.end()) {
		ELOG->critical("OutPort : {} does not exist in {}", out_port_id, GetName());
	}
	out_port_to_name_.erase(tp);
}

std::string BaseModel::GetInPortName(const PortId& port_id) {
	const auto iter = in_port_to_name_.find(port_id);
	if (iter != in_port_to_name_.end()) {
		return iter->second;
	}

	return fmt::format("unknown({})", port_id);
}

PortId BaseModel::GetInPortId(const std::string& in_port_name) {
	const auto iter = in_name_to_port_.find(in_port_name);
	if (iter == in_name_to_port_.end()) { return kErrorPort; }
	return iter->second;
}

std::string BaseModel::GetOutPortName(const PortId& port_id) {
	const auto iter = out_port_to_name_.find(port_id);
	if (iter != out_port_to_name_.end()) {
		return iter->second;
	}

	return fmt::format("unknown({})", port_id);
}

PortId BaseModel::GetOutPortId(const std::string& out_port_name) {
	const auto iter = out_name_to_port_.find(out_port_name);
	if (iter == out_name_to_port_.end()) { return kErrorPort; }
	return iter->second;
}

BaseModel* BaseModel::GetParent() const { return parent_model_; }

void BaseModel::ShowIoSummary() {
	ELOG->info("* Model name = {}", GetName());

	if (!in_port_to_name_.empty()) {
		ELOG->info("-- In Ports, Size = {}", in_port_to_name_.size());

		for (auto& each_in_port : in_port_to_name_)
			ELOG->info("   ({},{})", each_in_port.first, each_in_port.second);
	}

	if (!out_port_to_name_.empty()) {
		ELOG->info("-- Out Ports, Size = {}", out_port_to_name_.size());
		for (auto& each_out_port : out_port_to_name_)
			ELOG->info("   ({},{})", each_out_port.first, each_out_port.second);
	}
}

void BaseModel::ShowDestSummary() {
	if (!dest_port_pairs_.empty()) {
		ELOG->info("-- Dest Port information");

		for (auto& each_out_port_dest : dest_port_pairs_) {
			for (auto& each_dest : each_out_port_dest.second) {
				ELOG->info("   ({},{})-> ({},{})",
				           this->GetName(), GetOutPortName(each_out_port_dest.first),
				           each_dest.first->GetName(),
				           each_dest.first->GetInPortName(each_dest.second));
			}
		}
	}
}

void BaseModel::ResetDestModelPortPairs() {
	dest_port_pairs_.clear();
}


// inline bool BaseModel::SavePortDest(const PortId& out_port) {
//
// 	auto iter = dest_port_pairs_.find(out_port);
//
// 	if (iter == dest_port_pairs_.end()) {
// 		ELOG->critical(
// 				"The destinations for {}'s out port ({]) cannot be found in BaseModel::SavePortDest",
// 				GetName(), out_port);
// 		return false;
// 	}
//
// 	saved_dest_port_pairs_[out_port].clear();
// 	saved_dest_port_pairs_[out_port] = std::move(iter->second);
//
// 	return true;
// }
//
// inline bool BaseModel::RestorePostDest(const PortId& out_port) {
// 	auto iter = saved_dest_port_pairs_.find(out_port);
//
// 	if (iter == saved_dest_port_pairs_.end()) {
// 		ELOG->critical(
// 				"Saved destinations of {}'s out port ({]) cannot be found in BaseModel::RestorePortDest",
// 				GetName(), out_port);
// 		return false;
// 	}
//
// 	dest_port_pairs_[out_port].clear();
// 	dest_port_pairs_[out_port] = std::move(iter->second);
//
// 	return true;
// }
//

bool BaseModel::AddDestModelPort(const PortId& out_port,
                                 const DestPortPair& dest_port) {
	auto& dest_port_vec = dest_port_pairs_[out_port];
	const auto dest_iter = std::find(dest_port_vec.begin(), dest_port_vec.end(), dest_port);

	if (dest_iter == dest_port_vec.end()) {
		dest_port_vec.push_back(dest_port);
	}
	else {
		ELOG->critical(
			"OutPort : {}(in {}) already has a destination {}(of {}) in addDestModelPortPair",
			out_port, GetName(), dest_port.first->GetName(), dest_port.second);
		return false;
	}

	return true;
}

bool BaseModel::ReviseOutPortDest(const PortId& out_port, const DestPortPair& pre_dest_port,
                                  const DestPortPair& new_dest_port) {
	auto& dest_port_vec = dest_port_pairs_[out_port];
	const auto dest_iter = std::find(dest_port_vec.begin(), dest_port_vec.end(), pre_dest_port);

	if (dest_iter != dest_port_vec.end()) {
		*dest_iter = new_dest_port;
	}
	else {
		return false;
	}

	return true;
}


void BaseModel::RemoveDestModelPort(const PortId& out_port, const DestPortPair& dest_port_pair) {
	// add the dest model-port pair
	auto& dest_port_vec = dest_port_pairs_[out_port];
	const auto iter
		= std::find(dest_port_vec.begin(), dest_port_vec.end(), dest_port_pair);

	if (iter != dest_port_vec.end()) {
		dest_port_vec.erase(iter);
	}
	else {
		throw std::runtime_error("{}'s RemoveDestModelPort exception");
	}
}

// bool BaseModel::AddSavedDestModelPort(const PortId& out_port, const DestPortPair& dest_port) {
// 	const auto iter = saved_dest_port_pairs_.find(out_port);
// 	if( iter != saved_dest_port_pairs_.end()) {
// 		auto& saved_dest_vec = iter->second;
// 		const auto match_iter = std::find(saved_dest_vec.begin(), saved_dest_vec.end(), dest_port);
//
// 		if(match_iter != saved_dest_vec.end()) {
// 			ELOG->critical(
// 					"OutPort : {}(in {}) already has a destination {}(of {}) in addSavedDestModelPortPair",
// 					out_port, GetName(), dest_port.first->GetName(), dest_port.second);
// 			return false;
// 		}
//
// 		iter->second.push_back(dest_port);
// 	}
//
// 	return true;
// }

std::list<std::pair<BaseModel*, int>>& BaseModel::GetDestModelPortList(const PortId& out_port) {
	const auto iter = dest_port_pairs_.find(out_port);

	if (iter == dest_port_pairs_.end()) {
		ELOG->critical("OutPort : {}(in {}) does not exist in GetDestModelPortVec",
		               out_port, GetName());
	}

	return dest_port_pairs_[out_port];
}

bool BaseModel::IsDestExist(const PortId& out_port) {
	return !dest_port_pairs_[out_port].empty();
}

std::string BaseModel::ToInputString() const {
	std::string ret_str("[ ");

	for (const auto& each_input : in_port_to_name_) {
		ret_str += fmt::format("{}/{},", each_input.first, each_input.second);
	}

	ret_str += " ]";
	return ret_str;
}

std::string BaseModel::ToOutputString() const {
	std::string ret_str("[ ");

	for (const auto& each_output : out_port_to_name_) {
		ret_str += fmt::format("{}/{},", each_output.first, each_output.second);
	}

	ret_str += " ]";
	return ret_str;
}

std::string BaseModel::GetDestStr(const PortId& out_port) const {
	std::string ret_str;

	const auto iter = dest_port_pairs_.find(out_port);

	if (iter == dest_port_pairs_.end()) {
		return ret_str;
	}

	for (const auto& each_dest : iter->second) {
		const auto& src_model = each_dest.first;
		const auto& in_port_id = each_dest.second;
		ret_str += fmt::format("{} ({}/{}), ", src_model->GetName(), in_port_id,
		                       src_model->GetInPortName(in_port_id));
	}

	return ret_str;
}

bool BaseModel::IsInPortNameNew(const std::string& port_name) const {
	return !in_name_to_port_.contains(port_name);
}

bool BaseModel::IsOutPortNameNew(const std::string& port_name) const {
	return !out_name_to_port_.contains(port_name);
}

const Time& BaseModel::GetCurClock() const {
	return cur_clock_;
}
