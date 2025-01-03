// TEST FOR BASIC STINKY FUNCTIONALITY

#include "stinky/stinky.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(void) {
    ENetAddress adr;
    adr.host = ENET_HOST_ANY;
    adr.port = 6969;

    Stinky::Server * server = new Stinky::Server(adr, 8, 8, 0);

    constexpr int target_hz = 60;
    constexpr double target_duration_double = 1.0 / target_hz;
    constexpr auto target_duration = std::chrono::duration<double>(target_duration_double);

    std::chrono::steady_clock::time_point time0;
    std::chrono::steady_clock::time_point time1;
    for (;;) {
        time0 = std::chrono::steady_clock::now();

        server->Recv();

        time1 = std::chrono::steady_clock::now();
        auto elapsed = time1 - time0;
        auto sleeptime = target_duration - elapsed;
        std::this_thread::sleep_for(sleeptime);
    }

    delete(server);
    return 0;
}
