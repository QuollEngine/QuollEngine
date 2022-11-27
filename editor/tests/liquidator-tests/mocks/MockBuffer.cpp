#include "liquid/core/Base.h"
#include "MockBuffer.h"

MockBuffer::MockBuffer(const liquid::rhi::BufferDescription &description)
    : mData(const_cast<void *>(description.data)), mDescription(description) {}

void *MockBuffer::map() { return mData; }

void MockBuffer::unmap() {}

void MockBuffer::update(void *data_) { mData = data_; }

void MockBuffer::resize(size_t size) {}

const void *MockBuffer::getData() const { return mData; }

const size_t MockBuffer::getSize() const { return mDescription.size; }
