// STateful INternet Kit-Y
// This acronym sucks.
#pragma once

#include <raylib.h>
#include <enet/enet.h>
#include <sodium.h>
#include <thread>
#include <vector>

// Maximum message size in bytes
// This is not related to MTU, but rather the maximum packet size to decrypt. This is to stop DoS attacks against a server.
#define MAX_MESSAGE_SIZE 3000

namespace Stinky {

    class Host { // Abstract class that Server and Client derive from.
        public:
        protected:
            void Begin();
            // List of peers that are validated and safe to communicate with.
            std::vector<ENetPeer *> peers;

            // ENetHost container for client and server.
            ENetHost * host;

            // Seperate thread for the enet loop
            std::thread enet_thread;

            // Stop when var is true;
            bool enet_thread_stop_flag = false;

            // Event variable for Enet
            ENetEvent event;

            // Struct describing peer pairing information
            struct PeerInformation {
                bool keyExCompleted = false;

                //unsigned char * peer_pk[crypto_kx_PUBLICKEYBYTES];
                unsigned char * peer_pk;
                unsigned char rx_Sk[crypto_kx_SESSIONKEYBYTES];
                unsigned char tx_Sk[crypto_kx_SESSIONKEYBYTES];
            };

            struct Keys {
                // Since libsodium func calls behave differently whether we are client or host:
                bool isServer = false;
                // Host publickey and private key pair.
                unsigned char host_pk[crypto_kx_PUBLICKEYBYTES];
                unsigned char host_sk[crypto_kx_SECRETKEYBYTES];
            };

            // The hosts's keys.
            Keys * HostKeys;

            // Intiialize Enet and Libsodium subsystems.
            // Will return true on success, and false on failure. A failure should be considered unrecoverable.
            bool InitializeEnetAndCrypto();

            void RecvLoop(ENetHost * host, ENetEvent * event, bool * stopFlag, unsigned int waitTime);

    };

    class Server : private Host {
        private:
        public:
            // Reference is 32 clients, 8 channels, 0 (unlimited) bandwidth
            void Begin();
            void Stop();
            Server(ENetAddress address, enet_uint8 clients, enet_uint8 channels, enet_uint32 bandwidth);
            ~Server();

    };

    class Client : private Host {
        private:
            ENetAddress * serverAddress;
        public:
            void Begin();
            void Stop();
            // Reference is 1 outgoing connection, 8 channels, 0 (unlimited) bandwidth
            Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth);
            ~Client();
    };
}
