#pragma once
#include <map>
#include "message.h"

class SM_SIM_EXT_CLASS BaseDynCoupHandler {
public:
	BaseDynCoupHandler();
	virtual ~BaseDynCoupHandler();

	// delete
	BaseDynCoupHandler(const BaseDynCoupHandler&) = delete;
	BaseDynCoupHandler& operator =(const BaseDynCoupHandler&) = delete;
	BaseDynCoupHandler(const BaseDynCoupHandler&&) = delete;
	BaseDynCoupHandler& operator =(const BaseDynCoupHandler&&) = delete;

	void InitFlagToSrcOutPorts(
		const std::map<std::string, std::vector<std::pair<BaseModel*, PortId>>>&
		flag_to_model_ports);
	virtual bool HandleEvent(const Message& in_msg);

	virtual void PrintDynCoupling() const;

	std::map<std::string, std::vector<ModelPortPair>> flag_to_model_ports_;
};
