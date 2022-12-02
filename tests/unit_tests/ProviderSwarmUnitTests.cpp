#include <gtest/gtest.h>
#include "ProviderSwarm.hpp"

#include "../TestsCommon.hpp"

TEST(ProviderSwarmUnitTests, shouldCreate) {
    ASSERT_NO_THROW((ProviderSwarm<int>{{}}));
}

TEST(ProviderSwarmUnitTests, shouldCreateFromVector) {
    constexpr int expected_providers_count {5};
    std::vector<std::unique_ptr<Provider<int>>> providers {};

    for (int i = 0; i < expected_providers_count; i++) {
        providers.push_back(std::unique_ptr<Provider<int>>());
    }
    ProviderSwarm<int> swarm {std::move(providers)};
    int providers_count {0};

    for (const auto& _: swarm) {
        providers_count++;
    }
    ASSERT_EQ(providers_count, expected_providers_count);
    ASSERT_EQ(swarm.size(), expected_providers_count);
}

TEST(ProviderSwarmUnitTests, shouldAddProvider) {
    ProviderSwarm<int> swarm {{}};
    swarm.add(std::unique_ptr<Provider<int>>());

    ASSERT_EQ(swarm.size(), 1);
}

TEST(ProviderSwarmUnitTests, shouldEmplaceProvider) {
    ProviderSwarm<int> swarm {{}};
    swarm.emplace(std::weak_ptr<oneapi::tbb::concurrent_queue<int>>{}, GeneratorObj<int>{}, SenderObj<int>{});

    ASSERT_EQ(swarm.size(), 1);
}
