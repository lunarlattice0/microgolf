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

#include <cstdint>
#include <raylib.h>
#include <enet/enet.h>
#include <sodium.h>
#include <unordered_map>
#include <vector>
#include "packettypes.hpp"
#include <string>

// This is not related to MTU, but rather the maximum packet size to decrypt. This is to stop DoS attacks against a server.
#define MAX_MESSAGE_SIZE 3000

// Check packettypes.hpp for packet type designations.

// Note: What is the distinction between peers and players?
// Players are considered the abstracted version of a peer; PlayerInformation provides human-readable information like ID and nickname
// Peers are connected instances that are intended for use with ENet/FormatAndSend. Not recommended for use with human-readable implementations.
namespace Stinky {
    class Host { // Abstract class that Server and Client derive from.
        public:
            struct PlayerInformation {
                uint32_t id = 0;
                std::string nickname = "unnamed";
                template <class Archive> void serialize(Archive & archive) {
                    archive(id, nickname);
                }
            };
            void Recv();
            const std::unordered_map<uint32_t, ENetPeer*> GetPeersMap();
            const std::vector<ENetPeer*> GetPeersVector();
            const std::unordered_map<uint32_t, PlayerInformation> GetPlayersMap();
            const std::vector<PlayerInformation> GetPlayersVector();

            void FormatAndSend(PacketType pt, const ENetPeer * peer, enet_uint32 dataLen, unsigned char * data);
            PacketType DecryptAndFormat(const ENetPeer * peer, enet_uint32 receivedLen, unsigned char * received, unsigned char * decrypted);

            ENetPeer * FindPeerFromId(uint32_t id);
            uint32_t GetPlayerId();
        protected:
            bool retrySafe = true;
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

                uint32_t id = 0;
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

            // Dec 27, 2024: Switch to using own player tracking since it turns out ENet's system doesn't fit my needs.

            std::unordered_map<uint32_t, ENetPeer*> connectedPeers;
            // Don't send this over the network.

            std::unordered_map<uint32_t, PlayerInformation> connectedPlayers;
            // Send this over the network.
            void sendPlayers(); // Notify clients upon playerlist change.
            void receivePlayers(unsigned char *); // Update internal list of players.
            //
            uint32_t playerId = 0; // 0 on servers.
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
            uint32_t myId;
        public:
            // Reference is 1 outgoing connection, 8 channels, 0 (unlimited) bandwidth
            Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth);
            // Try to connect to a remote host.

            void AttemptConnect();
        };
}
