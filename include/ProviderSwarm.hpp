#pragma once

#include "Provider.hpp"
#include <vector>

template<typename T>
class ProviderSwarm {
public:
    explicit ProviderSwarm(std::vector<std::unique_ptr<Provider<T>>> providers)
            : providers(std::move(providers)) {}

    void start() {
        for (auto &provider: providers) {
            provider->start();
        }
    }

    void stop() {
        for (auto &provider: providers) {
            provider->stop();
        }
    }

    void add(std::unique_ptr<Provider<T>> provider) { providers.push_back(std::move(provider)); }

    template<typename ...Args>
    void emplace(Args &&...args) { providers.emplace_back(std::make_unique<Provider<T>>(std::forward<Args>(args)...)); }

    auto operator[](size_t i) noexcept {
        return providers[i];
    }

    auto operator[](size_t i) const noexcept {
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
                [](const auto &provider) { return provider->isRunning(); });
    }

private:
    std::vector<std::unique_ptr<Provider<T>>> providers;
};
