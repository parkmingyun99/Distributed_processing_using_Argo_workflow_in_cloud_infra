#pragma once

#include "base_ev.h"
#include "base_model.h"
#include <memory>

class SM_SIM_EXT_CLASS ModelSchEv final : public BaseEv {
public:
	/**
	 * \brief
	 * \param time: invoking reach_time
	 * \param src_model: source model
	 * \param src_model: destination model
	 * \param priority: sorting priority
	 */
	ModelSchEv(const Time& time, BaseModel* src_model);

	~ModelSchEv() override;

	ModelSchEv(const ModelSchEv&) = delete;

	ModelSchEv& operator=(const ModelSchEv&) = delete;

	ModelSchEv(const ModelSchEv&&) = delete;

	ModelSchEv& operator=(const ModelSchEv&&) = delete;

};


class SM_SIM_EXT_CLASS CoupChEv : public BaseEv {
public:
	CoupChEv(const CoupChEv& rhs) = default;

	CoupChEv(const Time& time, BaseModel* src_model, const int& priority, std::shared_ptr<void>& msg_ptr);

	~CoupChEv() override;

	// delete
	CoupChEv& operator=(const CoupChEv&) = delete;

	CoupChEv(const CoupChEv&&) = delete;

	CoupChEv& operator=(const CoupChEv&&) = delete;

	[[nodiscard]] void* GetMsg() const;

	[[nodiscard]] std::shared_ptr<void>& GetMsg();

	std::shared_ptr<void> msg_ptr_;

};


class SM_SIM_EXT_CLASS ExtEv : public BaseEv {
public:
	/**
	 * \brief
	 * \param time: invoking reach_time
	 * \param src_model: source model
	 * \param src_out_port: source port
	 * \param dest_model: receiver model
	 * \param dest_in_port: receiver port
	 * \param msg_ptr: value
	 */
	ExtEv(const ExtEv& rhs) = default;

	ExtEv(const Time& time, BaseModel* src_model, const PortId& src_out_port, BaseModel* dest_model,
	      const PortId& dest_in_port, std::shared_ptr<void>& msg_ptr);

	ExtEv(const Time& time, BaseModel* src_model, const PortId& src_out_port, BaseModel* dest_model,
	      const PortId& dest_in_port, const int& priority, std::shared_ptr<void>& msg_ptr);

	~ExtEv() override;

	// delete
	ExtEv& operator=(const ExtEv&) = delete;

	ExtEv(const ExtEv&&) = delete;

	ExtEv& operator=(const ExtEv&&) = delete;

	[[nodiscard]] PortId GetSrcOutPort() const;

	[[nodiscard]] PortId GetDestInPort() const;

	[[nodiscard]] void* GetMsg() const;

	[[nodiscard]] std::shared_ptr<void>& GetMsg();

	[[nodiscard]] void* GetValue() const { return msg_ptr_.get(); }

	[[nodiscard]] ExtEv* GetClone() const;

	std::shared_ptr<void> msg_ptr_;

};
