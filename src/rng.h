#pragma once

namespace {
    template<typename T = float>
    T uniformRand(float min, float max) {
        static std::random_device rd;
        static std::array<int, std::mt19937::state_size> seed_data;
        static bool generated = false;
        if (!generated) {
            std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
            generated = true;
        }
        static std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        static std::mt19937 gen(seq);

        std::uniform_real_distribution<T> dist(min, max);
        return dist(gen);
    }
}