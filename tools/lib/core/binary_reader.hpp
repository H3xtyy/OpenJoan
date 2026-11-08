// Bounds-checked little-endian reader over a byte span.
//
// The original engine read structures straight into memory with fread() and
// relied on #pragma pack(1) + x86 little-endian layout. We parse field by field
// instead so the code is portable (ARM, big-endian, 64-bit) and malformed data
// throws instead of corrupting memory.
#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

namespace joan {

class FormatError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class BinaryReader {
public:
    explicit BinaryReader(std::span<const std::byte> data) : data_(data) {}

    [[nodiscard]] std::size_t pos() const { return pos_; }
    [[nodiscard]] std::size_t size() const { return data_.size(); }
    [[nodiscard]] std::size_t remaining() const { return data_.size() - pos_; }
    [[nodiscard]] bool eof() const { return pos_ >= data_.size(); }

    void seek(std::size_t pos) {
        if (pos > data_.size()) throw FormatError("seek past end of data");
        pos_ = pos;
    }
    void skip(std::size_t n) { seek(pos_ + require(n)); }

    template <class T>
    T read() {
        static_assert(std::is_trivially_copyable_v<T>);
        require(sizeof(T));
        T value;
        std::memcpy(&value, data_.data() + pos_, sizeof(T));
        pos_ += sizeof(T);
        if constexpr (std::endian::native == std::endian::big && sizeof(T) > 1 && std::is_integral_v<T>) {
            value = byteswap(value);
        }
        return value;
    }

    std::uint8_t u8() { return read<std::uint8_t>(); }
    std::uint16_t u16() { return read<std::uint16_t>(); }
    std::uint32_t u32() { return read<std::uint32_t>(); }
    std::int16_t i16() { return read<std::int16_t>(); }
    std::int32_t i32() { return read<std::int32_t>(); }
    float f32() { return std::bit_cast<float>(u32()); }

    /// Reads a fixed-size, NUL-padded char field (e.g. `char name[9]`).
    std::string fixed_string(std::size_t field_len) {
        require(field_len);
        const auto* p = reinterpret_cast<const char*>(data_.data() + pos_);
        std::size_t n = 0;
        while (n < field_len && p[n] != '\0') ++n;
        pos_ += field_len;
        return std::string(p, n);
    }

    std::span<const std::byte> bytes(std::size_t n) {
        require(n);
        auto s = data_.subspan(pos_, n);
        pos_ += n;
        return s;
    }

private:
    std::size_t require(std::size_t n) const {
        if (n > data_.size() - pos_) throw FormatError("unexpected end of data");
        return n;
    }

    template <class T>
    static T byteswap(T v) {
        T r{};
        auto* src = reinterpret_cast<unsigned char*>(&v);
        auto* dst = reinterpret_cast<unsigned char*>(&r);
        for (std::size_t i = 0; i < sizeof(T); ++i) dst[i] = src[sizeof(T) - 1 - i];
        return r;
    }

    std::span<const std::byte> data_;
    std::size_t pos_ = 0;
};

inline std::string_view as_string_view(std::span<const std::byte> bytes) {
    return {reinterpret_cast<const char*>(bytes.data()), bytes.size()};
}

}  // namespace joan
