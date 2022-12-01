#include <gtest/gtest.h>
#include "Provider.hpp"

#include "../TestsCommon.hpp"

class SenderUnitTests : public CommonTestsFixture {};

TEST_F(SenderUnitTests, shouldCreate) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    ASSERT_NO_THROW((Provider<int>{queue, generator_func<int>, sender_func<int>}));
    ASSERT_EQ(GeneratorObj<int>::total_generated, 0);
    ASSERT_EQ(SenderObj<int>::total_sent, 0);
}

TEST_F(SenderUnitTests, shouldGenerateFuncSendFunc) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    Provider<int> sender {queue, generator_func<int>, sender_func<int>};
    sender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int res;
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(queue->try_pop(res));
    ASSERT_EQ(generator_func<int>(), res);
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
}

TEST_F(SenderUnitTests, shouldGenerateObjSendFunc) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    Provider<int> sender {queue, GeneratorObj<int>{}, sender_func<int>};
    sender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int res;
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(queue->try_pop(res));
    ASSERT_EQ(generator_func<int>(), res);
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
}

TEST_F(SenderUnitTests, shouldGenerateFuncSendObj) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    Provider<int> sender {queue, generator_func<int>, SenderObj<int>{}};
    sender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int res;
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(queue->try_pop(res));
    ASSERT_EQ(generator_func<int>(), res);
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
}

TEST_F(SenderUnitTests, shouldGenerateObjSendObj) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    Provider<int> sender {queue, GeneratorObj<int>{}, SenderObj<int>{}};
    sender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int res;
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(queue->try_pop(res));
    ASSERT_EQ(generator_func<int>(), res);
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
}

TEST_F(SenderUnitTests, shouldStopWhenQueueDestroyed) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    Provider<int> sender {queue, generator_func<int>, sender_func<int>};
    sender.start();
    queue.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_FALSE(sender.isRunning());
}
