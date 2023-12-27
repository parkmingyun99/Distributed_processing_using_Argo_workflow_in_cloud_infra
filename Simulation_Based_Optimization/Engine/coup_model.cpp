#include "coup_model.h"
#include <algorithm>
#include "logger.h"

CoupModel::CoupModel(): BaseModel(kCoupled) {
}

CoupModel::CoupModel(const std::string& name) : BaseModel(name, kCoupled) {

}

CoupModel::~CoupModel() {
	//Release
	std::for_each(child_models_.begin(), child_models_.end(), DeleteObj());
}

void CoupModel::MakeSubModels() {
	ELOG->critical("{}' MakeCoupling is not implemented", GetName());
	throw std::runtime_error("Exception in CoupModel::MakeCoupling");
}


void CoupModel::AddComponent(BaseModel* ptr_model) {
	if (IsChildModel(ptr_model)) {
		ELOG->critical("{} already exists in {}", ptr_model->GetName(), this->GetName());
	}
	else {
		child_models_.push_back(ptr_model);
		ptr_model->SetParent(this);
	}
}

void CoupModel::AddCoupling(BaseModel* from_model, const unsigned int& from_port,
                            BaseModel* to_model, const unsigned int& to_port) {
	if (from_model == this) {
		if (!from_model->HasInPort(from_port)) {
			ELOG->critical("{} coupling: {} does not have the input port {}", GetName(),
			               from_model->GetName(), from_port);

			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
	}
	else {
		if (!IsChildModel(from_model)) {
			ELOG->critical("{} coupling: {} does not have a src child model {}", GetName(),
			               GetName(), from_model->GetName());
			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
		if (!from_model->HasOutPort(from_port)) {
			ELOG->critical("{} coupling: {} does not have a out port ({},{})",
			               GetName(),
			               from_model->GetName(), from_port,
			               from_model->GetOutPortName(from_port));
			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
	}

	if (to_model == this) {
		if (!to_model->HasOutPort(to_port)) {
			ELOG->critical("{} coupling: {} does not have the out port {}",
			               GetName(), to_model->GetName(), to_port);
			ELOG->critical("{}'s output = {}", from_model->GetName(),
			               from_model->ToOutputString());
			ELOG->critical("{}'s input = {}", to_model->GetName(), to_model->ToOutputString());

			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
	}
	else {
		if (!IsChildModel(to_model)) {
			ELOG->critical("{} coupling: {} does not have a dest child model {}",
			               GetName(),
			               GetName(), to_model->GetName());
			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
		if (!to_model->HasInPort(to_port)) {
			ELOG->critical("{} coupling:{} does not have the input port ({},{})",
			               GetName(), to_model->GetName(), to_port,
			               to_model->GetInPortName(to_port));
			ELOG->critical("{}'s output = {}", from_model->GetName(),
			               from_model->ToOutputString());
			ELOG->critical("{}'s input = {}", to_model->GetName(),
			               to_model->ToOutputString());

			throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
			                                     from_model->GetName(), from_port,
			                                     to_model->GetName(), to_port));
		}
	}

	bool ret = true;

	if (const auto coupling = std::make_pair(std::make_pair(from_model, from_port),
	                                         std::make_pair(to_model, to_port)); from_model ==
		this) {
		ret = SaveCouplingToMap(cin_to_atm_in_, coupling);
	}
	else if (to_model == this) {
		ret = SaveCouplingToMap(atm_out_to_cont_out_, coupling);
	}
	else {
		ret = SaveCouplingToMap(atm_out_to_atm_in_, coupling);
	}

	if (!ret) {
		ELOG->critical("Coupling ({},{}->{},{}) already registered",
		               from_model->GetName(), from_port, to_model->GetName(), to_port);

		throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
		                                     from_model->GetName(), from_port,
		                                     to_model->GetName(), to_port));
	}
}

void CoupModel::AddCoupling(BaseModel* from_model, const std::string& from_port_name,
                            BaseModel* to_model, const std::string& to_port_name) {

	const auto from_port_id = (from_model == this)
		                          ? from_model->GetInPortId(from_port_name)
		                          : from_model->GetOutPortId(from_port_name);

	if (from_port_id == kErrorPort) {
		ELOG->critical("{} coupling: {} does not have a out port (whose name is {})",
		               this->GetName(), from_model->GetName(), from_port_name);
		throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
		                                     from_model->GetName(), from_port_name,
		                                     to_model->GetName(), to_port_name));
	}

	const auto to_port_id = (to_model == this)
		                        ? to_model->GetOutPortId(to_port_name)
		                        : to_model->GetInPortId(to_port_name);

	if (to_port_id == kErrorPort) {
		ELOG->critical("{} coupling: {} does not have a in src_port ({})", GetName(),
		               to_model->GetName(), to_port_name);
		throw std::runtime_error(fmt::format("Coupling problem: {},{} => {},{}",
		                                     from_model->GetName(), from_port_name,
		                                     to_model->GetName(), to_port_name));
	}

	AddCoupling(from_model, from_port_id, to_model, to_port_id);
}

bool CoupModel::IsChildModel(BaseModel* model) {
	auto it = child_models_.begin();
	for (; it != child_models_.end(); ++it)
		if (model == *it)
			return true;

	return false;
}

BaseModel* CoupModel::FindComponent(const std::string& name) {
	auto tp = child_models_.begin();
	for (; tp != child_models_.end(); ++tp) {
		if ((*tp)->GetName() == name) return *tp;
	}
	ELOG->critical("{} does not exist in ", name, this->GetName());
	return nullptr;
}

void CoupModel::SetPriority(int num, ...) {
	va_list ap;
	va_start(ap, num);
	auto i = 0;

	std::vector<BaseModel*> m_newChildComponent;

	while (num--) {
		auto m = va_arg(ap, BaseModel *);
		auto it = child_models_.begin();
		for (; it != child_models_.end(); ++it)
			if (m == *it)
				break;
		if (it == child_models_.end())
			ELOG->debug("The Model {} in SetPriorityFn doesn't exist in {}", m->GetName(),
			            this->GetName());
		m_newChildComponent.push_back(m);
		m->SetPriority(++i);
	}
	child_models_ = m_newChildComponent;
	m_newChildComponent.clear();

	va_end(ap);
}

void CoupModel::SetPriority(const std::vector<std::string>& models) {
	std::vector<BaseModel*> new_child_component;
	auto i = 0;

	for (const auto& model : models) {
		auto it = child_models_.begin();
		for (; it != child_models_.end(); ++it)
			if (model == (*it)->GetName())
				break;
		if (it == child_models_.end())
			ELOG->debug("The Model {} in SetPriorityFn doesn't exist in {}", model,
			            this->GetName());
		new_child_component.push_back(*it);
		(*it)->SetPriority(++i);
	}
	child_models_ = new_child_component;
	new_child_component.clear();
}

void CoupModel::ShowIoSummary() {
	ELOG->info("** Model name = {}", GetName());

	ELOG->info("   - In ports, Size = {}", in_port_to_name_.size());
	for (auto& each_in_port : in_port_to_name_)
		ELOG->info("   ({},{})", each_in_port.first, each_in_port.second);

	ELOG->info("   - Out ports, Size = {}", out_port_to_name_.size());
	for (auto& each_out_port : out_port_to_name_)
		ELOG->info("   ({},{})", each_out_port.first, each_out_port.second);

	ELOG->info("  * Child models, num = {}", child_models_.size());
	for (auto each_model : child_models_)
		ELOG->info("  ({})", each_model->GetName());
}

bool CoupModel::SaveCouplingToMap(
	std::map<ModelPortPair, std::vector<ModelPortPair>>& coup_map,
	const CouplingType& coupling) const {

	if (auto iter = coup_map.find(coupling.first); iter != coup_map.end()) {
		auto& dest_model_port_vec = iter->second;
		const auto dest_iter = std::find(dest_model_port_vec.begin(),
		                                 dest_model_port_vec.end(),
		                                 coupling.second);

		if (dest_iter == dest_model_port_vec.end()) {
			dest_model_port_vec.push_back(coupling.second);
		}
		else {
			return false;
		}
	}
	else {
		coup_map[coupling.first].push_back(coupling.second);
	}

	return true;
}
