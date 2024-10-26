// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <span>
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
        double time_ = 1;

        void test_to_bytes() {
            std::mt19937_64 rng;

            std::vector<std::array<CharT, 32>> in;
            for (int i = 0; i < 100'000; i++) {
                std::array<uint64_t, 2> a { rng(), rng() };
                std::array<CharT, 32> s;
                Hex::to_string(std::as_bytes(std::span(a)), s);
                in.push_back(s);
            }

            std::vector<std::array<ByteT, 16>> out(in.size());

            fqops::ops_measure ops("to_bytes\t" + name_, time_);
            ops.measure([&] (auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        Hex::to_bytes(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_to_string() {
            std::mt19937_64 rng;

            std::vector<std::array<ByteT, 16>> in;
            for (int i = 0; i < 100'000; i++) {
                std::array<uint64_t, 2> r { rng(), rng() };
                std::array<ByteT, 16> b;
                memcpy(b.data(), r.data(), b.size());
                in.push_back(b);
            }

            std::vector<std::array<CharT, 32>> out(in.size());

            fqops::ops_measure ops("to_string\t" + name_, time_);
            ops.measure([&] (auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        Hex::to_string(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

    public:
        explicit hex_perf_test(const std::string& s) : name_(s)  {}

        void run() {
            try {
                test_to_bytes();
                test_to_string();
            }
            catch (std::exception& e) {
                std::cerr << "error: " << e.what() << std::endl;;
            }
        }
    };
}
