// STateful INternet Kit-Y
// This acronym sucks.

/*
Basic setup:

Server:
1) Create a server object.
2) Call server's recv from raylib's mainloop.
3) Call destructor when done.

Client:
1) Create a server object
2) Call client recv's from mainloop
3) Call client's connect function from mainloop, but check if there are no peers first.
4) Call destructor when done.
*/

#pragma once

#include <raylib.h>
#include <enet/enet.h>
#include <sodium.h>
#include "packettypes.hpp"

// This is not related to MTU, but rather the maximum packet size to decrypt. This is to stop DoS attacks against a server.
#define MAX_MESSAGE_SIZE 3000

// Check packettypes.hpp for packet type designations.

namespace Stinky {
    class Host { // Abstract class that Server and Client derive from.
        public:
            void Recv();
            const ENetPeer * GetPeers();
            unsigned int GetPeersSize();
            // TODO: Implement and remove static.
            void FormatAndSend(PacketType pt, const ENetPeer * peer, enet_uint32 dataLen, unsigned char * data);
            PacketType DecryptAndFormat(const ENetPeer * peer, enet_uint32 receivedLen, unsigned char * received, unsigned char * decrypted);
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

                unsigned char * peer_pk; // stinky.cpp is in charge of making sure this is of the right size.
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
            ENetPeer * server;
        public:
            // Reference is 1 outgoing connection, 8 channels, 0 (unlimited) bandwidth
            Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth);
            // Try to connect to a remote host.

            // While a counter tracks if there is a connection in progress, don't spam this as you will end up wasting cycles.
            void AttemptConnect();
        };
}
