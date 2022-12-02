#pragma once

#include "Provider.hpp"
#include <vector>

template <typename T>
class ProviderSwarm {
public:
    explicit ProviderSwarm(std::vector<std::unique_ptr<Provider<T>>> providers)
        : providers(std::move(providers)) {
        assert(std::all_of(this->providers.begin(), this->providers.end(), [] (auto& obj) {return static_cast<bool>(obj);}));
        spdlog::debug("provider swarm created with {} providers", this->providers.size());
    }

    ~ProviderSwarm() {stop();}

    void start() {
        spdlog::debug("provider swarm starting");

        for (auto& provider: providers) {
            assert(provider);
            provider->start();
        }
        spdlog::debug("provider swarm started");
    }

    void stop() {
        spdlog::debug("provider swarm stopping");

        for (auto& provider: providers) {
            assert(provider);
            provider->stop();
        }
        spdlog::debug("provider swarm stopped");
    }

    void add(std::unique_ptr<Provider<T>> provider) {
        assert(provider);
        providers.push_back(std::move(provider));
    }

    template <typename ... Args>
    void emplace(Args&&... args) {providers.emplace_back(std::make_unique<Provider<T>>(std::forward<Args>(args)...));}

    auto operator [](size_t i) noexcept {
        return providers[i];
    }

    auto operator [](size_t i) const noexcept {
        return providers[i];
    }

    [[nodiscard]] size_t size() const noexcept {
        return providers.size();
    }

    auto begin() noexcept {
        return providers.begin();
    }

    auto begin() const noexcept {
        return providers.begin();
    }

    auto end() noexcept {
        return providers.end();
    }

    auto end() const noexcept {
        return providers.end();
    }

    [[nodiscard]] bool allAreRunning() const {
        return std::all_of(
            providers.begin(),
            providers.end(),
            [] (const auto& provider) {return provider->isRunning();});
    }

private:
    std::vector<std::unique_ptr<Provider<T>>> providers;
};
