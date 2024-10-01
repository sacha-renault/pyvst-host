#pragma once
#include <cstring>
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/funknown.h"

class MemoryStream : public Steinberg::IBStream {
public:
    MemoryStream() : data(nullptr), size(0), pos(0), refCount(1) {}
    MemoryStream(char* data, size_t size) : data(data), size(size), pos(0), refCount(1) {}

    // Override queryInterface
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) override {
        if (!obj) return Steinberg::kInvalidArgument;

        // If the requested interface matches IBStream or FUnknown, return this object
        if (std::memcmp(iid, Steinberg::FUnknown::iid, sizeof(Steinberg::TUID)) == 0 ||
            std::memcmp(iid, Steinberg::IBStream::iid, sizeof(Steinberg::TUID)) == 0) {
            *obj = static_cast<Steinberg::IBStream*>(this);
            addRef(); // Increase the reference count
            return Steinberg::kResultOk;
        }

        *obj = nullptr; // Interface not found
        return Steinberg::kNoInterface;
    }

    // Override addRef
    Steinberg::uint32 PLUGIN_API addRef() override {
        return ++refCount;
    }

    // Override release
    Steinberg::uint32 PLUGIN_API release() override {
        if (--refCount == 0) {
            delete this;
            return 0;
        }
        return refCount;
    }

    // Implement read method
    Steinberg::tresult PLUGIN_API read(void* buffer, Steinberg::int32 numBytes, Steinberg::int32* bytesRead) override {
        if (pos + numBytes > size) {
            numBytes = static_cast<Steinberg::int32>(size - pos);
        }
        memcpy(buffer, data + pos, numBytes);
        pos += numBytes;
        if (bytesRead) {
            *bytesRead = numBytes;
        }
        return numBytes > 0 ? Steinberg::kResultOk : Steinberg::kResultFalse;
    }

    // Implement write method (optional if not used)
    Steinberg::tresult PLUGIN_API write(void* buffer, Steinberg::int32 numBytes, Steinberg::int32* bytesWritten) override {
        // Presets are read-only for this example.
        return Steinberg::kNotImplemented;
    }

    // Implement seek method
    Steinberg::tresult PLUGIN_API seek(Steinberg::int64 pos, Steinberg::int32 mode, Steinberg::int64* result) override {
        Steinberg::int64 newPos = 0;
        switch (mode) {
            case Steinberg::IBStream::kIBSeekSet:
                newPos = pos;
                break;
            case Steinberg::IBStream::kIBSeekCur:
                newPos = this->pos + pos;
                break;
            case Steinberg::IBStream::kIBSeekEnd:
                newPos = size + pos;
                break;
            default:
                return Steinberg::kInvalidArgument;
        }

        if (newPos < 0 || newPos > size) {
            return Steinberg::kResultFalse;
        }

        this->pos = static_cast<size_t>(newPos);
        if (result) {
            *result = this->pos;
        }
        return Steinberg::kResultOk;
    }

    // Implement tell method
    Steinberg::tresult PLUGIN_API tell(Steinberg::int64* result) override {
        if (!result) {
            return Steinberg::kInvalidArgument;
        }
        *result = pos;
        return Steinberg::kResultOk;
    }

private:
    char* data;
    size_t size;
    size_t pos;
    Steinberg::uint32 refCount;
};