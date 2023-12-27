#include "base_dyn_coup_handler.h"
#include "logger.h"

BaseDynCoupHandler::BaseDynCoupHandler() = default;

BaseDynCoupHandler::~BaseDynCoupHandler() = default;

void BaseDynCoupHandler::InitFlagToSrcOutPorts(
	const std::map<std::string, std::vector<std::pair<BaseModel*, PortId>>>&
	flag_to_model_ports) {
	flag_to_model_ports_ = flag_to_model_ports;
}

bool BaseDynCoupHandler::HandleEvent(const Message& in_msg) {
	return false;
}

void BaseDynCoupHandler::PrintDynCoupling() const {
	ELOG->info("* Dyn Coupling info");

	for (const auto& each_item : flag_to_model_ports_) {
		ELOG->info("-- Flag = {}", each_item.first);

		for (const auto& each_pair : each_item.second) {
			ELOG->info("   ({},{})", each_pair.first->GetName(),
			           each_pair.first->GetOutPortName(each_pair.second));
		}
	}
}
