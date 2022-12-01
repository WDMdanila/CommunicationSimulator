#include <gtest/gtest.h>
#include "CommunicationFactory.hpp"

#include "../TestsCommon.hpp"

TEST(CommunicationFactoryUnitTests, shouldCreate) {
    ASSERT_NO_THROW((CommunicationFactory<int>{}));
}

TEST(CommunicationFactoryUnitTests, shouldCreatePair) {
    CommunicationFactory<int> factory {};
    auto [provider, requester] = factory.create(generator_func<int>, sender_func<int>, requester_func<int>);
    ASSERT_TRUE(provider);
    ASSERT_TRUE(requester);
}

TEST(CommunicationFactoryUnitTests, shouldCreateSwarm) {
    constexpr uint swarm_size {10};
    CommunicationFactory<int> factory {};
    auto [providers, requester] = factory.create_swarm(
        swarm_size,
        GeneratorObj<int>{},
        sender_func<int>,
        requester_func<int>);

    ASSERT_EQ(providers->size(), swarm_size);

    for (auto& provider: *providers) {
        ASSERT_TRUE(provider);
    }
    ASSERT_TRUE(requester);
}
