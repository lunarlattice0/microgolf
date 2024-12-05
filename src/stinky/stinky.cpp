// TODO
// we are STILL leaking data...

/*
Workflow:

If running server:
1) Create a Stinky::Server::Server() object
2) Tie RecvLoop to the game loop
3) After finishing, call Stinky::Server::Cleanup(). This disconnects all clients, but you can still call RecvLoop to reaccept clients.
4) Call destructor on Stinky::Server. This will destroy the host object, and is irrecoverable.

If running client:
1) Create a Stinky::Client::Client() object
2) Run Stinky::Client::PrepareConnect(). This attempts to connect to the server.
3) Run RecvLoop with waitTime of > 0. This will attempt to negotiate a connection with the server.
4) If RecvLoop returns, timedOut may be true or false.
5) Call Stinky::Client::Cleanup(). This will disconnect from the server, but you can repeat from step 2 to begin a new session.
6) Call destructor on Stinky::Client. This will destroy the host object, and is irrecoverable.
*/

#include "stinky/stinky.hpp"
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <raylib.h>
#include <sodium/crypto_secretbox.h>
#include <sstream>
#include <vector>

void Stinky::Host::FormatAndSend(ENetPeer * peer, DataPacket * dp) {
    // PacketType header + data
    PeerInformation * pi = static_cast<PeerInformation *>(peer->data);
    unsigned char nonce[crypto_secretbox_NONCEBYTES];

    unsigned char packet [sizeof(*dp)];
    std::memcpy(packet, dp, sizeof(*dp));

    unsigned char ciphertext [crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES + sizeof(packet)];

    randombytes_buf(nonce, sizeof nonce);

    crypto_secretbox_easy(&ciphertext[crypto_secretbox_NONCEBYTES], &packet[0], sizeof(packet), nonce, pi->tx_Sk);
    std::memcpy(&ciphertext[0], nonce, crypto_secretbox_NONCEBYTES);
    ENetPacket * enet_packet = enet_packet_create(&ciphertext[0], sizeof(ciphertext), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, enet_packet);

    // the packet is formatted as
    // NONCEBYTES + ciphertext.
}

void Stinky::Host::DecryptAndFormat(ENetPeer * peer, unsigned char * packet, DataPacket * dp) {
    PeerInformation * pi = static_cast<PeerInformation *>(peer->data);
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    std::memcpy(nonce, packet, crypto_secretbox_NONCEBYTES);
}

void Stinky::Server::RecvLoop() {
    Stinky::Host::RecvLoop(this->host, &this->event, 0);
}

void Stinky::Client::RecvLoop(unsigned int waitTime) {
    Stinky::Host::RecvLoop(this->host, &this->event, waitTime);
}
// Run me in a seperate thread!
void Stinky::Host::RecvLoop(ENetHost * host, ENetEvent * event, unsigned int waitTime) {
    // TODO: Why does this leak memory?
    while (enet_host_service(host, event, waitTime) > 0) {
        switch (event->type) {
            case ENET_EVENT_TYPE_CONNECT:
            {
                std::stringstream peerMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                peerMessage << "Peer connecting: " << std::string(friendlyHostIp) << ":" << event->peer->address.port << std::endl;
                TraceLog(LOG_INFO, peerMessage.str().c_str());

                // Now we need to send off our public key to the client and await their public key to complete key exchange.
                ENetPacket * packet = enet_packet_create(this->HostKeys->host_pk, crypto_kx_PUBLICKEYBYTES + 1, ENET_PACKET_FLAG_RELIABLE);

                enet_peer_send(event->peer, 0, packet);

                // Fill out a struct detailing the peer's information
                event->peer->data = static_cast<void*>(new PeerInformation);

                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Check if we're in key-exchange phase.
                PeerInformation * pi = static_cast<PeerInformation*>(event->peer->data);

                if (!pi->keyExCompleted) {
                    // Okay, we are, so try to calculate the session key.

                    // Make sure the packet received is the same length as the pubkey length.
                    if (event->packet->dataLength != crypto_kx_PUBLICKEYBYTES + 1) {
                        // eject the peer immediately.
                        enet_peer_reset(event->peer);
                        delete(pi);
                        event->peer->data = NULL;
                        goto destroy_packet;
                        //TODO: Cleanup this peer.
                    }


                    pi->peer_pk = event->packet->data;

                    if (this->HostKeys->isServer) {
                        if (crypto_kx_server_session_keys(
                            pi->rx_Sk,
                            pi->tx_Sk,
                            this->HostKeys->host_pk,
                            this->HostKeys->host_sk,
                            pi->peer_pk) != 0)
                        {
                            // Bad session key, eject client
                            std::stringstream badClientMessage;
                            badClientMessage << "A client sent a bad public key! Disconnecting." << std::endl;
                            TraceLog(LOG_INFO, badClientMessage.str().c_str());

                            enet_peer_reset(event->peer);
                            delete(pi);
                            event->peer->data = NULL;
                            goto destroy_packet;
                            break;
                        }
                    } else {
                        if (crypto_kx_client_session_keys(
                        pi->rx_Sk,
                        pi->tx_Sk,
                        this->HostKeys->host_pk,
                        this->HostKeys->host_sk,
                        pi->peer_pk) != 0)
                        {
                            // Bad session key, eject server

                            std::stringstream badServerMessage;
                            badServerMessage << "Server sent a bad public key! Disconnecting." << std::endl;
                            TraceLog(LOG_INFO, badServerMessage.str().c_str());

                            enet_peer_reset(event->peer);
                            delete(pi);
                            event->peer->data = NULL;
                            goto destroy_packet;
                            break;
                        }
                    }

                    // We're all good! Flag the key-exchange as complete.
                    pi->keyExCompleted = true;

                    std::stringstream keyExMessage;

                    char friendlyHostIp[64];

                    enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                    keyExMessage << "Negotiation success for: " << std::string(friendlyHostIp) << ":" << event->peer->address.port << std::endl;
                    TraceLog(LOG_INFO, keyExMessage.str().c_str());


                }
                // We're NOT in key-exchange phase, so treat data is gamedata
                // STUB
                destroy_packet:
                enet_packet_destroy(event->packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                std::stringstream disconnectMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                disconnectMessage << "Peer disconnecting: " << std::string(friendlyHostIp) << std::endl;
                TraceLog(LOG_INFO, disconnectMessage.str().c_str());
                delete(static_cast<PeerInformation*>(event->peer->data));

                event->peer->data = NULL;
                break;
            }
            case ENET_EVENT_TYPE_NONE:
            {
                // cool, nothing happened!

                break;
            }
        }
    }
    return;
};

bool Stinky::Host::InitializeEnetAndCrypto() {
    if (sodium_init() < 0) {
        TraceLog(LOG_ERROR, "Couldn't start libsodium! Bailing out...");
        return false;
    }

    if (enet_initialize() != 0) {
        TraceLog(LOG_ERROR, "Couldn't start ENet! Bailing out...");
        return false;
    }

    // Time to generate keys!
    this->HostKeys = new Keys{};

    crypto_kx_keypair(this->HostKeys->host_pk, this->HostKeys->host_sk);

    std::atexit(enet_deinitialize);
    return true;
}

void Stinky::Client::PrepareConnect() {
    ENetPeer * peer = enet_host_connect(this->host, this->serverAddress, this->host->channelLimit, 0);
    if (peer == NULL) {
        TraceLog(LOG_INFO, "A connection couldn't be made to the server!");
        return;
    }
}


Stinky::Server::Server(ENetAddress address, enet_uint8 clients, enet_uint8 channels, enet_uint32 bandwidth) {
    if (!this->InitializeEnetAndCrypto()) {
        throw std::runtime_error("A subsystem failed to start!");
    }

    this->HostKeys->isServer = true;

    this->host = enet_host_create(&address,
        clients,
        channels,
        bandwidth,
        bandwidth
    );

    if (host == NULL) {
        TraceLog(LOG_ERROR, "ENet Server failed to start! Bailing out...");
        throw std::runtime_error("Server constructor failed! Check logs.");
    }

}

Stinky::Server::~Server() {
    this->Cleanup();
    delete(this->HostKeys);
    enet_host_destroy(this->host);
}

void Stinky::Server::Cleanup() {
    for (unsigned i = 0; i < this->host->connectedPeers; ++i) {
        auto peer = &this->host->peers[i];
        enet_peer_disconnect_now(peer, 0);
        delete(static_cast<PeerInformation*>(peer->data));
    }
}

Stinky::Client::Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth) {
    if (!this->InitializeEnetAndCrypto()) {
        throw std::runtime_error("A subsystem failed to start!");
    }

    this->HostKeys->isServer = false;
    this->host = enet_host_create(NULL, outgoing, channels, bandwidth, bandwidth);

    if (host == NULL) {
        TraceLog(LOG_ERROR, "ENet client failed to start! Bailing out...");
        throw std::runtime_error("Client constructor failed! Check logs.");
    }

    this->serverAddress = serverAddress;

}
void Stinky::Client::Cleanup() {
    for (unsigned i = 0; i < this->host->connectedPeers; ++i) {
        auto peer = &this->host->peers[i];
        enet_peer_disconnect_now(peer, 0);
        delete(static_cast<PeerInformation*>(peer->data));
    }
}

Stinky::Client::~Client() {
    this->Cleanup();
    delete(this->HostKeys);
    enet_host_destroy(this->host);
}
