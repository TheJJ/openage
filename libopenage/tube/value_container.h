// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube_internal.h"


namespace openage {
namespace tube {

/**
 * Base class for storing data values that change over time.
 */
template<typename _T>
class ValueContainer {
protected:
	Tube<_T> container;
	mutable tubeelement<_T> *e_now;
	mutable tube_time_t now;

public:
	ValueContainer();
	virtual ~ValueContainer() = default;

	// Reader mode
	virtual void set_now(const tube_time_t &t);

	virtual _T at() const = 0;
	virtual _T at(const tube_time_t &t) const;

	/**
	 * Test if this position has data at the given time.
	 */
	virtual bool needs_update(const tube_time_t &at);

	/**
	 * Set a keyframe at the given time.
	 * Forget all following keyframes.
	 */
	void set_end(const tube_time_t &at, const _T &value);

	/**
	 * Insert a keyframe at the given time.
	 */
	void set_insert(const tube_time_t &at, const _T &value);
};


template <typename _T>
ValueContainer<_T>::ValueContainer()
	:
	e_now{nullptr},
	now{} {}


template <typename _T>
void ValueContainer<_T>::set_now(const tube_time_t &t) {
	this->now = t;
	this->e_now = this->container.last(t, e_now);
}


template <typename _T>
_T ValueContainer<_T>::at(const tube_time_t &t) const {
	this->set_now(t);
	return get();
}


template <typename _T>
void ValueContainer<_T>::set_end(const tube_time_t &at, const _T &value) {
	this->container.erase_after(container.last(at, e_now));
	this->container.create(at, value);
}


template <typename _T>
void ValueContainer<_T>::set_insert(const tube_time_t &at, const _T &value) {
	this->container.create(at, value);
}


template <typename _T>
bool ValueContainer<_T>::needs_update(const tube_time_t &at) {
	// TODO take container.end as a hint?
	auto e = this->container.last(at, this->e_now);
	if (e->time > at || e->next == nullptr || e->next->time > at) {
		return true;
	} else {
		return false;
	}
}

}} // openage::tube
