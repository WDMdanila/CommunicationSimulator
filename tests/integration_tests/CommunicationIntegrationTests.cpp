#include <gtest/gtest.h>
#include "../TestsCommon.hpp"
#include "CommunicationFactory.hpp"

class CommunicationIntegrationTests : public CommonTestsFixture {};

TEST_F(CommunicationIntegrationTests, shouldCommunicate) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues_list = std::vector {queue};
    Provider<int> sender {queue, generator_func<int>, sender_func<int>};
    Requester<int> requester {queues_list, requester_func<int>};

    sender.start();
    requester.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(requester.isRunning());
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

TEST_F(CommunicationIntegrationTests, shouldCommunicateMultipleSenders) {
    auto queue = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queue_2 = std::make_shared<oneapi::tbb::concurrent_queue<int>>();
    auto queues_list = std::vector {queue, queue_2};
    Provider<int> sender {queue, generator_func<int>, sender_func<int>};
    Provider<int> sender_2 {queue_2, generator_func<int>, sender_func<int>};
    Requester<int> requester {queues_list, requester_func<int>};

    sender.start();
    sender_2.start();
    requester.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(sender.isRunning());
    ASSERT_TRUE(sender_2.isRunning());
    ASSERT_TRUE(requester.isRunning());
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

TEST_F(CommunicationIntegrationTests, shouldCommunicateSwarm) {
    constexpr uint swarm_size {2};
    CommunicationFactory<int> factory {};
    auto [providers, requester] = factory.create_swarm(
        swarm_size,
        generator_func<int>,
        sender_func<int>,
        requester_func<int>);

    providers->start();
    requester->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_TRUE(providers->allAreRunning());
    ASSERT_TRUE(requester->isRunning());
    ASSERT_GT(GeneratorObj<int>::total_generated, 0);
    ASSERT_GT(SenderObj<int>::total_sent, 0);
    ASSERT_GT(RequesterObj<int>::total_requested, 0);
}

class CustomObject {
public:
    int &operator *() {return *data;}

private:
    std::unique_ptr<int> data {std::make_unique<int>(1)};
};

TEST_F(CommunicationIntegrationTests, shouldCommunicateSwarmObject) {
    constexpr uint swarm_size {2};
    CommunicationFactory<CustomObject> factory {};
    auto [providers, requester] = factory.create_swarm(
        swarm_size,
        generator_func<CustomObject>,
        sender_func<CustomObject>,
        requester_func<CustomObject>);

    providers->start();
    requester->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_TRUE(providers->allAreRunning());
    ASSERT_TRUE(requester->isRunning());
    ASSERT_GT(GeneratorObj<CustomObject>::total_generated, 0);
    ASSERT_GT(SenderObj<CustomObject>::total_sent, 0);
    ASSERT_GT(RequesterObj<CustomObject>::total_requested, 0);
}
