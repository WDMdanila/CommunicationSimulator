#include "CommunicationFactory.hpp"
#include "Utilities.hpp"

int main() {
#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::debug);
#endif

    CommunicationFactory<uint> factory {};

    spdlog::info("creating swarm");
    auto [swarm, requester] = factory.create_swarm(10, DefaultGenerator {}, DefaultSender{}, DefaultReceiver{});
    spdlog::info("created swarm, starting");

    swarm->emplace(requester->getChannel(), DefaultGenerator {}, DefaultSender{});

    swarm->start();
    requester->start();

    spdlog::info("started, waiting for second");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    spdlog::info("done waiting, stopping");
    swarm->stop();
    requester->stop();
    spdlog::info("total generated: {}", DefaultGenerator::counter.load());
    spdlog::info("total sent: {}", DefaultSender::counter.load());
    spdlog::info("total received: {}", DefaultReceiver::counter.load());

    return 0;
}
