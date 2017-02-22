// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "tube.h"

#include <iostream>
#include <limits>
#include <cassert>

namespace openage {
namespace tube {


/**
 * A timely ordered list with several management functions
 *
 * This class manages different time-based management functions for the double-
 * linked list approach that lies underneath. It contains a double-linked list
 * to be accessed via a non-accurate timing functionality, this means, that for
 * getting a value, not the exact timestamp has to be known, it will always return
 * the one closest, less or equal to the requested one.
 */
template <typename _T>
class KeyframeContainer {
public:
	/**
	 * A element of the double-linked list KeyframeContainer
	 */
	class Keyframe {
	private:
		Keyframe(const tube_time_t &time)
			:
			time{time} {}

		Keyframe(const tube_time_t &time, const _T &value)
			:
			time{time},
			value{value} {}

	public:
		Keyframe<_T> *next = nullptr;
		Keyframe<_T> *prev = nullptr;

		const tube_time_t time = 0;
		_T value = _T();
	};

public:
	KeyframeContainer();

	~KeyframeContainer();

	/**
	 * Get the last element with e->time <= time.
	 */
	Keyframe<_T> *last(const tube_time_t &time, Keyframe<_T>* hint=nullptr) const;

	/**
	 * Create a new element and insert it into the tree.
	 * TODO create a pool where it might be possible to draw memory from.
	 */
	Keyframe<_T> *create(const tube_time_t &, const _T& value);

	/**
	 * Insert a newly ekement into the tree, that has not yet been inserted.
	 */
	void insert(Keyframe<_T> *, Keyframe<_T> *hint=nullptr);

	/**
	 * Erase the whole list after this element until the end.
	 */
	void erase_after(Keyframe<_T> *last_valid);

	/**
	 * Remove the Keyframe from its container.
	 */
	void erase(Keyframe<_T> *e);

private:
	/**
	 * Points to the first valid element.
	 */
	Keyframe<_T> *begin = nullptr;

	/**
	 * Points to the last valid element.
	 */
	Keyframe<_T> *end = nullptr;
};



template<typename _T>
KeyframeContainer<_T>::KeyframeContainer() {
	// Create a default element at -Inf, that can always be dereferenced so
	// there will by definition never be a element that cannot be dereferenced
	create(-std::numeric_limits<tube_time_t>::infinity(), _T());
}


template<typename _T>
KeyframeContainer<_T>::~KeyframeContainer() {
	this->erase_after(this->begin);
	delete this->begin;
}


/**
 * Select the element that directly preceedes the given timestamp.
 *
 * Without a hint, start to iterate at the beginning of the buffer, and return
 * the element last element before e->time > time.
 * This method returns nullptr, if begin->time > time.
 */
template <typename _T>
Type::Keyframe<_T> *KeyframeContainer<_T>::last(const tube_time_t &time, Type::Keyframe<_T> *hint) const {
	Type::Keyframe<_T> *e = hint ? hint : this->begin;

	if (e == nullptr) {
		return e;
	}

	if (this->begin->time > time) {
		// This will never happen due to the begin->time == -Inf magic!
		throw Error{ERR << "internal error: there is always a element at -inf"};
	}

	// Search in the queue
	if (time > e->time) { // the searched element is supposed to be AFTER the hint
		// perform forward search
		while (e->next != nullptr && time >= e->next->time) {
			e = e->next;
		}
		// e is now one of two options:
		// 1. e == end: The last element of the queue was smaller than `time`
		// 2. e != end: There was a element with `e->time` > `time`
	}
	else if (time < e->time) {
		// perform backward search as time is
		while (e->prev != nullptr && time < e->time) {
			e = e->prev;
		}
		// e is now one of two options:
		// 1. e == begin: The time was before every element in the queue
		// 2. e != begin: There was an element with `e->time` > `time`
	}
	else {
		// time == e->time, thus return the hint/begin.
	}

	return e;
}


/**
 * Create and insert a new element into this tube
 */
template <typename _T>
Type::Keyframe<_T> *KeyframeContainer<_T>::create(const tube_time_t &time, const _T& value) {
	// TODO this has to be managed by a memory pool!
	auto e = new Type::Keyframe<_T>(time, value);
	this->insert(e);
	return e;
}


/**
 * Determine where to insert, and update all references
 */
template <typename _T>
void KeyframeContainer<_T>::insert(Type::Keyframe<_T> *e, Type::Keyframe<_T> *hint) {
	// There are no elements in the list right now.
	if (begin == nullptr) {
		begin = e;
		end = e;
		return;
	}

	Type::Keyframe<_T>* at = last(e->time, hint);

	// if "last" cannot point at a location, so there was no element _before_
	// the newly inserted
	if (at == nullptr) {
		begin->prev = e;
		e->next = begin;
		begin = e;
	} else if (at->next == nullptr || end == at) {
		// if next is nullptr, then it has to be at the end, so update the end
		at->next = e;
		e->prev = at;
		end = e;
	} else {
		// the list is not empty, it is not at the beginning, it is not at the end:
		// it has to be in the middle! so we can perform a normal insert
		e->next = at->next;
		e->prev = at;
		at->next->prev = e;
		at->next = e;
	}
}


/**
 * Go from the end to the last_valid element, and call erase on all of them
 */
template <typename _T>
void KeyframeContainer<_T>::erase_after(Type::Keyframe<_T> *last_valid) {
	Type::Keyframe<_T> *e = end;
	//Delete from the end to last_valid
	while (e != nullptr && e != last_valid) {
		Type::Keyframe<_T> *prev = e->prev;
		erase(e);
		e = prev;
	}
}


/**
 * Delete the element from the list and call delete on it.
 */
template <typename _T>
void KeyframeContainer<_T>::erase(Type::Keyframe<_T> *e) {
	if (e == nullptr) return;
	if (begin == e) {
		begin = e->next;
	}
	if (end == e) {
		end = e->prev;
	}

	if (e->next != nullptr) {
		e->next->prev = e->prev;
	}
	if (e->prev != nullptr) {
		e->prev->next = e->next;
	}

	delete e; // TODO Memory management magic!
}

}} // openage::tube
