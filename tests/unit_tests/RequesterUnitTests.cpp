#include <gtest/gtest.h>
#include "Requester.hpp"

#include "../TestsCommon.hpp"

class RequesterUnitTests : public CommonTestsFixture {};

TEST_F(RequesterUnitTests, shouldCreate) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues = std::vector {queue};
    ASSERT_NO_THROW((Requester<int>{queues, requester_func<int>}));
}

TEST_F(RequesterUnitTests, shouldRequestFunc) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues = std::vector {queue};
    Requester<int> requester {queues, requester_func<int>};
    requester.start();
    queue->push(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(queue->empty());
    ASSERT_TRUE(requester.isRunning());
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

TEST_F(RequesterUnitTests, shouldRequestObj) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues = std::vector {queue};
    Requester<int> requester {queues, RequesterObj<int>{}};
    requester.start();
    queue->push(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(queue->empty());
    ASSERT_TRUE(requester.isRunning());
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

TEST_F(RequesterUnitTests, shouldRequestFromTwoSources) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queue_2 = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues = std::vector {queue, queue_2};
    Requester<int> requester {queues, RequesterObj<int>{}};
    requester.start();
    queue->push(1);
    queue_2->push(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(queue->empty());
    ASSERT_TRUE(queue_2->empty());
    ASSERT_TRUE(requester.isRunning());
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

TEST_F(RequesterUnitTests, shouldReturnChannel) {
    Requester<int> requester {{}, RequesterObj<int>{}};
    auto channel_weak = requester.getChannel();
    auto channel = channel_weak.lock();
    ASSERT_FALSE(channel_weak.expired());
    ASSERT_TRUE(channel);
}
