#pragma once

#include <gtest/gtest.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <optional>
template <typename T>
T generator_func();
template <typename T>
void sender_func(oneapi::tbb::concurrent_queue<T>& queue, T val);
template <typename T>
void requester_func(oneapi::tbb::concurrent_queue<T>& queue);

template <typename T>
struct GeneratorObj {
    T operator ()() {return generator_func<T>();}

    static inline std::atomic_uint total_generated {0};
};

template <typename T>
struct SenderObj {
    void operator ()(oneapi::tbb::concurrent_queue<T>& queue, T&& val) {sender_func<T>(queue, std::forward<T>(val));}

    static inline std::atomic_uint total_sent {0};
};

template <typename T>
struct RequesterObj {
    void operator ()(oneapi::tbb::concurrent_queue<T>& queue) {
        requester_func<T>(queue);
        total_requested++;
    }

    static inline std::atomic_uint total_requested {0};
};

template <typename T>
T generator_func() {
    GeneratorObj<T>::total_generated++;
    return T {};
}

template <typename T>
void sender_func(oneapi::tbb::concurrent_queue<T>& queue, T val) {
    queue.push(std::move(val));
    SenderObj<T>::total_sent++;
}

template <typename T>
void requester_func(oneapi::tbb::concurrent_queue<T>& queue) {
    std::optional<T> val;
    queue.try_pop(*val);
    RequesterObj<T>::total_requested++;
}

class CommonTestsFixture : public testing::Test {
protected:
    void SetUp() override {
        GeneratorObj<int>::total_generated = 0;
        SenderObj<int>::total_sent = 0;
        RequesterObj<int>::total_requested = 0;
    }
};
