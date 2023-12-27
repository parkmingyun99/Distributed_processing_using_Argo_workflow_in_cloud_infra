#include "coup_plant.h"
#include "logger.h"
#include <ranges>

CoupPlant::CoupPlant(const std::deque<int>& jobs) : job_seq_(jobs) {
	SetName("Plant");
}

CoupPlant::~CoupPlant() = default;

void CoupPlant::MakeSubModels() {
#if ! defined(NDEBUG) || defined(OPT_LOG_TRACE_ENABLE)
	MLOG->info(" @ {}: 'Jobs' = {}", name_, fmt::join(job_seq_, ","));
#endif
	auto* const generator = new Generator(job_seq_);
	AddComponent(generator);
	auto* const trans = new Transducer(job_seq_);
	AddComponent(trans);

	auto* const cell1 = new Cell(CellType::kCell1, 2);
	AddComponent(cell1);
	auto* const cell2_1 = new Cell(CellType::kCell2_1, 1);
	AddComponent(cell2_1);
	auto* const cell2_2 = new Cell(CellType::kCell2_2, 1);
	AddComponent(cell2_2);
	auto* const cell3 = new Cell(CellType::kCell3, 1);
	AddComponent(cell3);

	auto* const agv = new Agv();
	AddComponent(agv);

	const std::vector<BaseModel*> fetch_group = { generator, cell1, cell2_1, cell2_2, cell3 };
	const std::vector<BaseModel*> dep_group = { cell1, cell2_1, cell2_2, cell3, trans };

	// coupling avail
	AddCoupling(cell1, "Avail", generator, "Avail");
	AddCoupling(cell2_1, "Avail", cell1, "Avail");
	AddCoupling(cell2_2, "Avail", cell1, "Avail");
	AddCoupling(cell3, "Avail", cell2_1, "Avail");
	AddCoupling(cell3, "Avail", cell2_2, "Avail");

	for (const auto& each_model : fetch_group) {
		AddCoupling(agv, fmt::format("ArrFetch{}", each_model->GetName()), each_model, "ArrFetch");
		AddCoupling(each_model, "ReqToAgv", agv, "ReqFromCell");
		AddCoupling(each_model, "ReqCancel", agv, "ReqCancel");
	}

	for (const auto& each_model : dep_group) {
		AddCoupling(agv, fmt::format("ArrDep{}", each_model->GetName()), each_model, "ArrDep");
	}
}
