// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include "ops_measure.hpp"

namespace fqhex
{
    template <class Hex>
    class hex_perf_test
    {
        using CharT = Hex::char_type;
        using ByteT = Hex::byte_type;

        std::string name_;
        std::string type_name_;
        double time_ = 1;

        std::string get_header(const std::string& method, size_t size) {
            std::ostringstream ss;
            ss << method << "\t"
               << type_name_ << "\t"
               << size << "\t"
               << name_;
            return ss.str();
        }

        template <size_t ByteSize>
        void test_to_bytes() {
            constexpr auto CharSize = ByteSize * 2;
            std::mt19937_64 rng;

            std::vector<std::array<CharT, CharSize>> in;
            for (int i = 0; i < 100'000; i++) {
                std::array<uint64_t, 2> a { rng(), rng() };
                std::array<CharT, CharSize> s;
                auto b = std::as_bytes(std::span(a)).first(CharSize / 2);
                Hex::to_string(b, s);
                in.push_back(s);
            }

            std::vector<std::array<ByteT, ByteSize>> out(in.size());

            auto header = get_header("to_bytes", CharSize);
            fqops::ops_measure ops(header, time_);
            ops.measure([&] (auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        Hex::to_bytes(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

        template <size_t ByteSize>
        void test_to_string() {
            constexpr auto CharSize = ByteSize * 2;
            std::mt19937_64 rng;

            std::vector<std::array<ByteT, ByteSize>> in;
            for (int i = 0; i < 100'000; i++) {
                std::array<uint64_t, 2> r { rng(), rng() };
                std::array<ByteT, ByteSize> b;
                memcpy(b.data(), r.data(), b.size());
                in.push_back(b);
            }

            std::vector<std::array<CharT, CharSize>> out(in.size());

            auto header = get_header("to_string", CharSize);
            fqops::ops_measure ops(header, time_);
            ops.measure([&] (auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        Hex::to_string(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

        using test_fn = void(hex_perf_test::*)();

        static constexpr test_fn tests[] = {
            &hex_perf_test::test_to_bytes<16>,
            &hex_perf_test::test_to_bytes<8>,
            &hex_perf_test::test_to_bytes<4>,
            &hex_perf_test::test_to_bytes<2>,
            &hex_perf_test::test_to_string<16>,
            &hex_perf_test::test_to_string<8>,
            &hex_perf_test::test_to_string<4>,
            &hex_perf_test::test_to_string<2>,
        };

    public:
        explicit hex_perf_test(const std::string& name, const std::string& type_name)
            : name_(name), type_name_(type_name)  {}

        void run() {
            for (auto test : tests) {
                try {
                    (this->*test)();
                }
                catch (std::exception& e) {
                    std::cerr << "error: " << e.what() << std::endl;;
                }
            }
        }
    };
}
