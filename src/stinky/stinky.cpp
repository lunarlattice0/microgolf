// TODO
// Something is desperately broken with disconnect messages, the client is not sending them at all.
// we are STILL leaking data...

#include "stinky/stinky.hpp"
#include <enet/enet.h>
#include <iostream>
#include <raylib.h>
#include <sstream>

// Run me in a seperate thread!
void Stinky::Host::RecvLoop(ENetHost * host, ENetEvent * event, bool * stopFlag) {
    while (enet_host_service(host, event, 0) >= 0 && *stopFlag == false) {
        switch (event->type) {
            case ENET_EVENT_TYPE_CONNECT:
            {
                std::stringstream peerMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                peerMessage << "Peer connecting: " << std::string(friendlyHostIp) << ":" << event->peer->address.port << std::endl;
                TraceLog(LOG_INFO, peerMessage.str().c_str());

                // Now we need to send off our public key to the client and await their public key to complete key exchange.
                ENetPacket * packet = enet_packet_create(this->HostKeys->host_pk, crypto_kx_PUBLICKEYBYTES, ENET_PACKET_FLAG_RELIABLE);

                enet_peer_send(event->peer, 0, packet);

                // Fill out a struct detailing the peer's information
                PeerInformation * pi = new PeerInformation(); // Why does this cause a memory leak???

                event->peer->data = static_cast<void*>(pi);

                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Check if we're in key-exchange phase.
                PeerInformation * pi = static_cast<PeerInformation*>(event->peer->data);

                if (!pi->keyExCompleted) {
                    // Okay, we are, so try to calculate the session key.

                    // Make sure the packet received is the same length as the pubkey length.
                    if (event->packet->dataLength != crypto_kx_PUBLICKEYBYTES) {
                        // eject the peer immediately.
                        enet_peer_reset(event->peer);
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
                        }
                    }

                    // We're all good! Flag the key-exchange as complete.
                    pi->keyExCompleted = true;
                    this->peers.push_back(event->peer);

                    std::stringstream keyExMessage;

                    char friendlyHostIp[64];

                    enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                    keyExMessage << "Negotiation success for: " << std::string(friendlyHostIp) << ":" << event->peer->address.port << std::endl;
                    TraceLog(LOG_INFO, keyExMessage.str().c_str());

                    break;
                }
                // We're NOT in key-exchange phase, so treat data is gamedata
                // STUB

                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                // Why aren't Disconnect messages being sent???
                std::stringstream disconnectMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event->peer->address, &friendlyHostIp[0], 64);
                disconnectMessage << "Peer disconnecting: " << std::string(friendlyHostIp) << std::endl;
                TraceLog(LOG_INFO, disconnectMessage.str().c_str());
                delete(static_cast<PeerInformation*>(event->peer->data));

                // Iterators cause weird behavior here. Why?
                for (unsigned long i = 0; i < this->peers.size(); ++i) {
                    if (event->peer == this->peers[i]) {
                        this->peers.erase(peers.begin() + i);
                    }
                }


                break;
            }
            case ENET_EVENT_TYPE_NONE:
            {
                // cool, nothing happened!
                break;
            }
        }
    }
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

// These aren't available in the superclass, since we need to be sure that the constructor has been called first.
void Stinky::Server::Begin() {
    this->enet_thread_stop_flag = false;
    RecvLoop(this->host, &this->event, &this->enet_thread_stop_flag);
}

void Stinky::Client::Begin() {
    this->enet_thread_stop_flag = false;
    ENetPeer * peer = enet_host_connect(this->host, this->serverAddress, this->host->channelLimit, 0);
    if (peer == NULL) {
        TraceLog(LOG_INFO, "A connection couldn't be made to the server!");
        return;
    }
    RecvLoop(this->host, &this->event, &this->enet_thread_stop_flag);
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
    this->Stop();
    delete(this->HostKeys);
    enet_host_destroy(this->host);
}

void Stinky::Server::Stop() {
    this->enet_thread_stop_flag = true;
    // Cleanly shut down first
    for (auto it = std::begin(this->peers); it != std::end(this->peers); ++it) {
        enet_peer_disconnect_later(*it, 0);
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
void Stinky::Client::Stop() {
    this->enet_thread_stop_flag = true;
    for (auto it = std::begin(this->peers); it != std::end(this->peers); ++it) {
        enet_peer_disconnect_later(*it, 0);
    }
}

Stinky::Client::~Client() {
    this->Stop();
    delete(this->HostKeys);
    enet_host_destroy(this->host);
}
