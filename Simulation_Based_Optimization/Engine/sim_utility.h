#pragma once
#include "date/date.h"
#include "sim_type.h"
#include "lib_engine.h"
#include "base_model.h"

// time_point conversion
SM_SIM_EXT_CLASS date::sys_seconds Convert(const std::string& date_time_str);
SM_SIM_EXT_CLASS std::string GetString(const date::sys_seconds& time_var);
SM_SIM_EXT_CLASS std::string GetString(const ModelType& model_type);
SM_SIM_EXT_CLASS std::string GetModelName(const BaseModel* model);
SM_SIM_EXT_CLASS std::string GetInPortId(BaseModel* model, const int& in_port);
SM_SIM_EXT_CLASS std::string GetStringFromMsg(const void* ptr_msg);
