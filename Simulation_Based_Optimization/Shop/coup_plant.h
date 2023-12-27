#pragma once
#include "lib_sm_model.h"
#include "coup_model.h"
#include "generator.h"
#include "agv.h"
#include "Cell.h"
#include "transducer.h"

class SM_MODEL_CLASS CoupPlant : public CoupModel {
public:
	explicit  CoupPlant(const std::deque<int>& jobs);
	virtual ~CoupPlant();

	// delete
	CoupPlant(const CoupPlant&) = delete;
	CoupPlant& operator =(const CoupPlant&) = delete;
	CoupPlant(const CoupPlant&&) = delete;
	CoupPlant& operator =(const CoupPlant&&) = delete;
	
	void MakeSubModels() override;

protected:
	std::string fab_name_;
    std::deque<int> job_seq_;
};
