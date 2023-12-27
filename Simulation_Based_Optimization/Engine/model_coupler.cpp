#include "model_coupler.h"
#include "logger.h"
#include <deque>

ModelCoupler::ModelCoupler() = default;

ModelCoupler::~ModelCoupler() = default;


bool ModelCoupler::FlattenModel(CoupModel* out_most_coup_model) {
	// opt_data_duplicated_ = false;
	unknown_coup_out_to_ain_.clear();

	MakeGenComponents(out_most_coup_model);

	MakeCoupling(out_most_coup_model);

	//all couplings are settled?
	if (!unknown_coup_out_to_ain_.empty()) {
		ShowUnConnSource();
		return false;
	}
	return true;
}

void ModelCoupler::MakeCoupling(CoupModel* coup_model) {
	auto& child_vec = coup_model->child_models_;

	for (auto* each_child : child_vec) {
		if (each_child->GetType() == kCoupled) {
			MakeCoupling(dynamic_cast<CoupModel*>(each_child));
		}
		else {
			each_child->ResetDestModelPortPairs();

			// add the dynamic port
			if (each_child->HasDynPort()) {
				for (const auto& [flag, out_port_set] : each_child->dyn_out_ports_) {
					auto& src_out_port_vec = flag_to_src_out_port_pairs_[flag];
					src_out_port_vec.insert(std::end(src_out_port_vec),
					                        out_port_set.begin(), out_port_set.end());
				}
			}
		}
	}

	ReflectCoupling(coup_model->cin_to_atm_in_, EIC);
	ReflectCoupling(coup_model->atm_out_to_atm_in_, IIC);
	ReflectCoupling(coup_model->atm_out_to_cont_out_, IEC);

	IdentifyCoupling();
}

void ModelCoupler::MakeGenComponents(CoupModel* coup_model) {
	// make child models
	coup_model->MakeSubModels();
	auto& child_vec = coup_model->child_models_;

	for (auto* each_child : child_vec) {
		if (each_child->GetType() == kCoupled) {
			MakeGenComponents(dynamic_cast<CoupModel*>(each_child));
		}
	}
}

void ModelCoupler::ShowUnConnSource() {
	for (auto& each_item : unknown_coup_out_to_ain_) {
		auto src_model_port_pair = each_item.first;
		const int port_id = src_model_port_pair.second;
		ELOG->warn("Coupled model ({})'s input port {} is not connected",
		           src_model_port_pair.first->GetName(),
		           src_model_port_pair.first->GetInPortName(port_id));
	}
}

bool ModelCoupler::IdentifyCoupling() {

	UpdateUnknownCoupOut();
	UpdateUnknownCoupIn();

	return true;
}

void ModelCoupler::UpdateUnknownCoupOut() {
	for (auto iter = unknown_coup_out_to_ain_.cbegin();
	     iter != unknown_coup_out_to_ain_.cend();) {

		std::vector<ModelPortPair> src_atomic_model_port_pairs;

		auto unknown_coup_source = iter->first;
		GetSubAmsOfCoupOutSrc(unknown_coup_source, src_atomic_model_port_pairs);

		if (!src_atomic_model_port_pairs.empty()) {

			for (const auto& each_dest : iter->second) {
				for (const auto& each_src : src_atomic_model_port_pairs) {
					each_src.first->AddDestModelPort(
						each_src.second, each_dest);
				}
			}

			// delete the identified source of coupled model 
			iter = unknown_coup_out_to_ain_.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void ModelCoupler::UpdateUnknownCoupIn() {
	for (auto iter = unknown_coup_in_to_ain_.cbegin();
	     iter != unknown_coup_in_to_ain_.cend();) {

		std::vector<ModelPortPair> src_atomic_model_port_pairs;

		auto unknown_coup_source = iter->first;
		GetSubAmsOfCoupInSrc(unknown_coup_source, src_atomic_model_port_pairs);

		if (!src_atomic_model_port_pairs.empty()) {
			for (const auto& each_dest : iter->second) {
				for (const auto& each_src : src_atomic_model_port_pairs) {
					each_src.first->AddDestModelPort(
						each_src.second, each_dest);
				}
			}

			// delete the identified source of coupled model 
			iter = unknown_coup_in_to_ain_.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void ModelCoupler::GetSubAmsOfCoupOutSrc(const ModelPortPair& coupled_source,
                                         std::vector<ModelPortPair>& src_atomic_vec) {
	std::deque<ModelPortPair> coup_dest_queue;
	coup_dest_queue.push_back(coupled_source);

	while (!coup_dest_queue.empty()) {
		const auto tmp_dest_pair = coup_dest_queue.front();
		coup_dest_queue.pop_front();

		auto iter = dest_out_to_src_out_.find(tmp_dest_pair);
		if (iter != dest_out_to_src_out_.end()) {
			const auto& src_model_port_pairs = iter->second;

			for (const auto& each_pair : src_model_port_pairs) {
				if (each_pair.first->GetType() != kCoupled) {
					src_atomic_vec.push_back(each_pair);
				}
				else {
					if (dest_out_to_src_out_.contains(each_pair)) {
						coup_dest_queue.push_back(each_pair);
					}
				}
			}
		}
	}
}

void ModelCoupler::GetSubAmsOfCoupInSrc(const ModelPortPair& dest_model_port_pair,
                                        std::vector<ModelPortPair>& src_atomic_vec) {

	const auto iter = dest_in_to_src_out_.find(dest_model_port_pair);

	if (iter != dest_in_to_src_out_.end()) {
		// add the previous atomic models 
		for (const auto& each_src : iter->second) {
			if (each_src.first->GetType() != kCoupled) {
				src_atomic_vec.push_back(each_src);
			}
			else {
				GetSubAmsOfCoupOutSrc(each_src, src_atomic_vec);
			}
		}
	}
}

void ModelCoupler::ReflectCoupling(
	const std::map<ModelPortPair, std::vector<ModelPortPair>>& coupling,
	const CoupType& coup_type) {

	for (auto& each_src_dest_pair : coupling) {
		auto each_src = each_src_dest_pair.first;

		for (auto& each_dest : each_src_dest_pair.second) {
			const auto src_type = each_src.first->GetType();
			const auto dest_type = each_dest.first->GetType();

			if (src_type == kCoupled || dest_type == kCoupled) {

				if (coup_type == EIC) {
					auto& ain_dest_vec = unknown_coup_in_to_ain_[each_src];

					if (dest_type != kCoupled) {
						ain_dest_vec.push_back(each_dest);
					}
					else {
						auto iter = unknown_coup_in_to_ain_.find(each_dest);
						if (iter != unknown_coup_in_to_ain_.end()) {
							auto& dest_ain_vec = iter->second;
							ain_dest_vec.insert(ain_dest_vec.end(), dest_ain_vec.begin(),
							                    dest_ain_vec.end());
							unknown_coup_in_to_ain_.erase(iter);
						}
					}
				}
				else if (coup_type == IIC) {
					dest_in_to_src_out_[each_dest].push_back(each_src);

					if (dest_type != kCoupled) {
						unknown_coup_out_to_ain_[each_src].push_back(each_dest);
					}
				}
				else {
					dest_out_to_src_out_[each_dest].push_back(each_src);
				}
			}
			else {
				each_src.first->AddDestModelPort(each_src.second, each_dest);
			}
		}
	}


}
