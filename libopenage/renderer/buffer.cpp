// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "buffer.h"

namespace openage {
namespace renderer {

Buffer::Buffer(Context *ctx, size_t size)
	:
	context{ctx},
	allocd{size} {

	if (size > 0) {
		this->create(size);
	}
}

size_t Buffer::size() const {
	return this->allocd;
}

char *Buffer::get() const {
	return this->buffer.get();
}

void Buffer::create(size_t size) {
	this->buffer = std::make_unique<char[]>(size);
	this->allocd = size;
}

}} // openage::renderer