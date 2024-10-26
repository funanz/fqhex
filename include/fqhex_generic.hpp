// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>

namespace fqhex
{
    template <class CharT, class ByteT>
    class hex_generic
    {
    public:
        using char_type = CharT;
        using byte_type = ByteT;

    private:
        static constexpr auto hex_to_u4_table = [] {
            std::array<int8_t, 256> a;
            for (size_t i = 0; i < a.size(); i++) {
                if (0x30 <= i && i <= 0x39) // [0-9]
                    a[i] =  i - 0x30;
                else if (0x41 <= i && i <= 0x46) // [A-F]
                    a[i] =  i - 0x41 + 10;
                else if (0x61 <= i && i <= 0x66) // [a-f]
                    a[i] =  i - 0x61 + 10;
                else
                    a[i] = -1;
            }
            return a;
        }();

        static constexpr uint_fast32_t hex_to_u4(CharT c) {
            if constexpr (sizeof(c) > 1) {
                if (static_cast<size_t>(c) > 0xff)
                    return -1;
            }

            static_assert(hex_to_u4_table.size() == 256);
            return hex_to_u4_table[static_cast<size_t>(c) & 0xff];
        }

        static constexpr auto u4_to_hex_table = [] {
            std::array<CharT, 16> a;
            for (int i = 0; i < 10; i++)
                a[i] = 0x30 + i; // [0-9]
            for (int i = 0; i < 6; i++)
                a[10 + i] = 0x61 + i; // [a-f]
            return a;
        }();

        static constexpr CharT u4_to_hex(ByteT u4) {
            static_assert(u4_to_hex_table.size() == 16);

            return u4_to_hex_table[static_cast<size_t>(u4) & 0xf];
        }

    public:
        static constexpr void to_bytes(std::span<const CharT> in, std::span<ByteT> out) {
            if (out.size() * 2 < in.size())
                throw std::invalid_argument("output size is small");

            auto count = in.size() / 2;
            for (size_t i = 0; i < count; i++) {
                auto u4e = (hex_to_u4(in[i * 2]) << 4 |
                            hex_to_u4(in[i * 2 + 1]));
                if (u4e > 0xff)
                    throw std::invalid_argument("invalid hex character");

                out[i] = static_cast<ByteT>(u4e);
            }
        }

        static constexpr void to_string(std::span<const ByteT> in, std::span<CharT> out) {
            if (out.size() < in.size() * 2)
                throw std::invalid_argument("output size is small");

            for (size_t i = 0; i < in.size(); i++) {
                out[i * 2] = u4_to_hex(in[i] >> 4);
                out[i * 2 + 1] = u4_to_hex(in[i]);
            }
        }

        static auto to_string(std::span<const ByteT> in) {
            std::basic_string<CharT> s(in.size() * 2, 0);
            to_string(in, s);
            return s;
        }
    };
}
