#include <gtest/gtest.h>
#include "ProviderSwarm.hpp"

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
