#pragma once

#include "base_model.h"
#include <map>

class SM_SIM_EXT_CLASS CoupModel : public BaseModel {
public:
	using CouplingType = std::pair<ModelPortPair, ModelPortPair>;
	std::vector<BaseModel*> child_models_{};

	CoupModel();
	explicit CoupModel(const std::string& name);
	~CoupModel() override;

	virtual void MakeSubModels();

	void AddComponent(BaseModel*);
	void AddCoupling(BaseModel*, const unsigned int&, BaseModel*, const unsigned int&);
	void AddCoupling(BaseModel*, const std::string&, BaseModel*, const std::string&);

	/*void RemoveComponent(UdeBaseModel *);
	void RemoveCoupling(UdeBaseModel *, unsigned int, UdeBaseModel *, unsigned int);
*/
	bool IsChildModel(BaseModel*);
	BaseModel* FindComponent(const std::string& name);

	void SetPriority(int num, ...);
	void SetPriority(const std::vector<std::string>& models);

	void ShowIoSummary() override;

private:
	bool SaveCouplingToMap(std::map<ModelPortPair, std::vector<ModelPortPair>>& coup_map,
	                       const CouplingType& coupling) const;

public:
	// coupled model to inner atomic
	std::map<ModelPortPair, std::vector<ModelPortPair>> cin_to_atm_in_{};
	std::map<ModelPortPair, std::vector<ModelPortPair>> atm_out_to_atm_in_{};
	std::map<ModelPortPair, std::vector<ModelPortPair>> atm_out_to_cont_out_{};

};
