#pragma once
#include "coup_model.h"

/**
 * \brief : connect the model
 */
class ModelCoupler final {
public:
	ModelCoupler();
	~ModelCoupler();

	bool FlattenModel(CoupModel* out_most_coup_model);

private:
	enum CoupType { EIC, IIC, IEC };

	void MakeCoupling(CoupModel* coup_model);
	static void MakeGenComponents(CoupModel* coup_model);

	void ShowUnConnSource();

	//APIs for make coupling
	bool IdentifyCoupling();
	void UpdateUnknownCoupOut();
	void UpdateUnknownCoupIn();

	void GetSubAmsOfCoupOutSrc(const ModelPortPair& coupled_source,
	                           std::vector<ModelPortPair>& src_atomic_vec);
	void GetSubAmsOfCoupInSrc(const ModelPortPair& dest_model_port_pair,
	                          std::vector<ModelPortPair>& src_atomic_vec);


	void ReflectCoupling(
		const std::map<ModelPortPair, std::vector<ModelPortPair>>& coupling,
		const CoupType& coup_type);

public:
	std::map<ModelPortPair, std::vector<ModelPortPair>> unknown_coup_in_to_ain_;
	std::map<ModelPortPair, std::vector<ModelPortPair>> unknown_coup_out_to_ain_;

	std::map<ModelPortPair, std::vector<ModelPortPair>> dest_in_to_src_out_;
	std::map<ModelPortPair, std::vector<ModelPortPair>> dest_out_to_src_out_;

	std::map<std::string, std::vector<ModelPortPair>> flag_to_src_out_port_pairs_;
};
