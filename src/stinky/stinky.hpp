// STateful INternet Kit-Y
// This acronym sucks.
#pragma once

#include <enet/types.h>
#include <raylib.h>
#include <enet/enet.h>
#include <sodium.h>
#include "packettypes.hpp"

// Maximum message size in bytes
// This is not related to MTU, but rather the maximum packet size to decrypt. This is to stop DoS attacks against a server.
#define MAX_MESSAGE_SIZE 3000

namespace Stinky {
    // Check packettypes.hpp for packet type designations.
    class Host { // Abstract class that Server and Client derive from.
        public:
            void Recv();
            static void FormatAndSend(ENetPeer * peer, PacketType pt, enet_uint32 dataLen, unsigned char * data);
            static unsigned char * DecryptAndFormat(ENetPacket * dp, ENetPeer * peer, unsigned char * result);
        protected:
            Host();
            ~Host();

            // ENetHost container for client and server.
            ENetHost * host;

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
    };

    class Server : public Host {
        public:
            // Reference is 32 clients, 8 channels, 0 (unlimited) bandwidth
            Server(ENetAddress address, enet_uint8 clients, enet_uint8 channels, enet_uint32 bandwidth);
    };

    class Client : public Host {
        private:
            ENetAddress * serverAddress;
        public:
            // Reference is 1 outgoing connection, 8 channels, 0 (unlimited) bandwidth
            Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth);
    };
}
