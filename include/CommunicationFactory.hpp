#pragma once

#include "ProviderSwarm.hpp"
#include "Requester.hpp"

template <typename T>
class CommunicationFactory {
public:
    template <typename G, typename S, typename R>
    std::pair<std::unique_ptr<Provider<T>>, std::unique_ptr<Requester<T>>> create(G&& generator,
        S&& sender,
        R&& requester) {
        auto queue = std::make_shared<oneapi::tbb::concurrent_queue<T>>();
        auto queues = std::vector {queue};
        return {std::make_unique<Provider<T>>(queue, std::forward<G>(generator), std::forward<S>(sender)),
                std::make_unique<Requester<T>>(queues, std::forward<R>(requester))};
    }

    template <typename G, typename S, typename R>
    std::pair<std::unique_ptr<ProviderSwarm<T>>, std::unique_ptr<Requester<T>>> create_swarm(uint num,
        G generator,
        S sender,
        R&& requester) {
        static_assert(std::is_copy_constructible_v<G>, "Generator must be copy constructible");
        static_assert(std::is_copy_constructible_v<S>, "Sender must be copy constructible");
        std::vector<std::unique_ptr<Provider<T>>> providers {};
        std::vector<std::shared_ptr<oneapi::tbb::concurrent_queue<T>>> queues {};

        for (uint i {0}; i < num; i++) {
            auto queue = std::make_shared<oneapi::tbb::concurrent_queue<T>>();
            queues.push_back(queue);
            providers.emplace_back(
                std::make_unique<Provider<T>>(
                    std::move(queue),
                    generator,
                    sender));
        }
        return {std::make_unique<ProviderSwarm<T>>(std::move(providers)),
                std::make_unique<Requester<T>>(queues, std::forward<R>(requester))};
    }
};
