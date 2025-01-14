// TODO: When connecting two clients, disconnecting the first client causes the second client's peer to be mangled (but not corrupted). Why?
#include "stinky/stinky.hpp"
#include "packettypes.hpp"
#include <cereal/details/helpers.hpp>
#include <cstdint>
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <raylib.h>
#include <sodium/randombytes.h>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>
#include <string>
#include <vector>

uint32_t Stinky::Host::GetPlayerId() {
    return this->playerId;
}

const std::unordered_map<uint32_t, Stinky::Host::PlayerInformation> Stinky::Host::GetPlayersMap() {
    return this->connectedPlayers;
}
const std::vector<Stinky::Host::PlayerInformation> Stinky::Host::GetPlayersVector() {
    std::vector<PlayerInformation> vec;
    for (auto it : this->GetPlayersMap()) {
        vec.push_back(it.second);
    }
    return vec;
}

void Stinky::Host::sendPlayers() {
    if (this->HostKeys->isServer) {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive oarchive(ss);
            oarchive(this->connectedPlayers);
        }
        for (auto it : this->connectedPeers) {
            this->FormatAndSend<char>(MG_PLAYERLIST, it.second, ss.str().length() + 1, ss.str().data());
            //this->FormatAndSend(MG_PLAYERLIST, it.second, ss.str().length() + 1, reinterpret_cast<unsigned char *>(ss.str().data()));
        }
        for (auto it : this->connectedPeers) {
            std::string id = std::to_string(it.first);

            this->FormatAndSend<char>(MG_WHOAMI, it.second, id.length() + 1, id.data());
            //this->FormatAndSend(MG_WHOAMI, it.second, id.length() + 1, reinterpret_cast<unsigned char *>(id.data()));
        }
        return;
    }
}

void Stinky::Host::receivePlayers(unsigned char * gameData) {
    if (!this->HostKeys->isServer) {
        std::string gd(reinterpret_cast<char *>(gameData));
        std::stringstream ss(gd);
        {
            cereal::JSONInputArchive iarchive(ss);
            iarchive(this->connectedPlayers);
        }
    }
    return;
}

void Stinky::Host::FormatAndSend(PacketType pt, const ENetPeer * peer, enet_uint32 dataLen, unsigned char * data) {
    PeerInformation * pi = static_cast<PeerInformation *>(peer->data);

    // The data is sent in the following format:
    // nonce (NONCEBYTES), [packettype(unsignedchar = 8 bits), remaining data]
    // For convenience sake, the section after the nonce will be referred to as datasection

    // prepare some libsodium stuff
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    unsigned char * datasection = new unsigned char[sizeof(pt) + dataLen];
    std::memcpy(datasection, &pt, sizeof(pt));
    std::memcpy(datasection + sizeof(pt), data, dataLen);


    // datasection is now filled.

    // we create a temporary buffer to store nonce and ciphertext

    unsigned char * ciphertext = new unsigned char[crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES + sizeof(pt) + dataLen];

    // copy nonce to front of ciphertext
    std::memcpy(ciphertext, nonce, crypto_secretbox_NONCEBYTES);

    // now encrypt the datasection, but offset
    crypto_secretbox_easy(ciphertext + crypto_secretbox_NONCEBYTES, datasection, sizeof(PacketType)+dataLen, nonce, pi->tx_Sk);

    // send off the packet
    ENetPacket * enet_packet = enet_packet_create(ciphertext, crypto_secretbox_NONCEBYTES+crypto_secretbox_MACBYTES+sizeof(pt)+dataLen, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(const_cast<ENetPeer *>(peer), 0, enet_packet);

    // Destroy the stuff on the heap
    delete[] ciphertext;
    delete[] datasection;

    return;
}

// Use what enet says is the packet size for receivedLen field.
// decrypted MUST be at least (receivedLen - crypto_box_NONCEBYTES - crypto_box_MACBYTES - sizeof(PacketType))
PacketType Stinky::Host::DecryptAndFormat(const ENetPeer * peer, enet_uint32 receivedLen, unsigned char * received, unsigned char * decrypted) {
    PeerInformation * pi = static_cast<PeerInformation *>(peer->data);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    std::memcpy(nonce, received, crypto_box_NONCEBYTES);

    unsigned char * datasection = new unsigned char[receivedLen - crypto_box_NONCEBYTES - crypto_box_MACBYTES];

    if (crypto_secretbox_open_easy(datasection, received + crypto_box_NONCEBYTES, receivedLen - crypto_box_NONCEBYTES, nonce, pi->rx_Sk) != 0) {
        return MG_ERROR;
    } else {

        PacketType pt;
        std::memcpy(&pt, datasection, sizeof(PacketType));
        std::memcpy(decrypted, datasection + sizeof(PacketType), receivedLen - crypto_box_NONCEBYTES - crypto_box_MACBYTES - sizeof(PacketType));

        delete[] datasection;
        return pt;
    }
}


void Stinky::Host::Recv() {
    while (enet_host_service(this->host, &this->event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
            {
                std::stringstream peerMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event.peer->address, &friendlyHostIp[0], 64);
                peerMessage << "Peer connecting: " << std::string(friendlyHostIp) << ":" << event.peer->address.port << std::endl;
                TraceLog(LOG_INFO, peerMessage.str().c_str());

                // Now we need to send off our public key to the client and await their public key to complete key exchange.
                ENetPacket * packet = enet_packet_create(this->HostKeys->host_pk, crypto_kx_PUBLICKEYBYTES + 1, ENET_PACKET_FLAG_RELIABLE);

                enet_peer_send(event.peer, 0, packet);

                // Fill out a struct detailing the peer's information
                event.peer->data = static_cast<void*>(new PeerInformation);

                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                // Check if we're in key-exchange phase.
                PeerInformation * pi = static_cast<PeerInformation*>(event.peer->data);

                if (!pi->keyExCompleted) {
                    // Okay, we are, so try to calculate the session key.

                    // Make sure the packet received is the same length as the pubkey length.
                    if (event.packet->dataLength != crypto_kx_PUBLICKEYBYTES + 1) {
                        // eject the peer immediately.
                        enet_peer_reset(event.peer);
                        delete(pi);
                        event.peer->data = NULL;
                        goto destroy_packet;
                    }


                    pi->peer_pk = event.packet->data;

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

                            enet_peer_reset(event.peer);
                            delete(pi);
                            event.peer->data = NULL;
                            goto destroy_packet;
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

                            enet_peer_reset(event.peer);
                            delete(pi);
                            event.peer->data = NULL;
                            goto destroy_packet;
                        }
                    }

                    // We're all good! Flag the key-exchange as complete.
                    pi->keyExCompleted = true;

                    std::stringstream keyExMessage;

                    char friendlyHostIp[64];

                    enet_address_get_host_ip(&event.peer->address, &friendlyHostIp[0], 64);
                    keyExMessage << "Negotiation success for: " << std::string(friendlyHostIp) << ":" << event.peer->address.port << std::endl;
                    TraceLog(LOG_INFO, keyExMessage.str().c_str());

                    // Track this peer.
                    pi->id = randombytes_random();
                    this->connectedPeers[pi->id] = event.peer;
                    auto currentEpochTime = std::time(0);
                    connectedPlayers[pi->id] = PlayerInformation{
                        .id = pi->id,
                        .nickname = "unnamed",
                        .lastMessageTime = currentEpochTime,
                        .lastNicknameChangeTime = currentEpochTime,
                    };
                    // Server only: Send playerlist to everyone on new player join.
                    sendPlayers();
                    goto destroy_packet;
                }

                // We're NOT in key-exchange phase, so treat data as gamedata
                {
                    auto gameDataTrueLength = event.packet->dataLength - crypto_box_NONCEBYTES - crypto_box_MACBYTES - sizeof(PacketType);
                    unsigned char * gameData = new unsigned char [gameDataTrueLength];
                    PacketType gamePacketType = this->DecryptAndFormat(event.peer, event.packet->dataLength, event.packet->data, gameData);

                    switch (gamePacketType) {
                        case MG_ERROR:
                            {
                                TraceLog(LOG_ERROR, "Discarding invalid packet.");
                                break;
                            }
                        /*
                        case MG_TEST:
                            {
                                std::cout << std::string(reinterpret_cast<char *>(gameData)) << std::endl;
                                break;
                            }
                        */
                        case MG_WHOAMI:
                            {
                                auto playerIdStr = reinterpret_cast<char *>(gameData);
                                this->playerId = static_cast<uint32_t>(std::stoul(playerIdStr));
                                break;
                            }
                        case MG_PLAYERLIST:
                            {
                                receivePlayers(gameData);
                                break;
                            }
                        case MG_NICKNAME_CHANGE: // for server use
                            {
                                auto currentEpochTime = std::time(0);
                                if (HostKeys->isServer && (currentEpochTime - this->connectedPlayers[pi->id].lastNicknameChangeTime > MIN_NICKNAME_CHANGE_DELAY)) {
                                    char buf[32];
                                    if (gameDataTrueLength > 32) { // concatenate name
                                        std::memcpy(buf, gameData, 31);
                                        buf[31] = '\0'; // make sure the last char is null termed
                                    } else {
                                        std::memcpy(buf, gameData, gameDataTrueLength); // bytes 0 -30 are nickname
                                        buf[gameDataTrueLength - 1] = '\0'; // make sure the last char is null termed
                                    }

                                    this->connectedPlayers[pi->id].nickname = std::string(buf);
                                    TraceLog(LOG_INFO, "Set nickname for %zu to %s", pi->id, this->connectedPlayers[pi->id].nickname.c_str());
                                    sendPlayers();
                                    this->connectedPlayers[pi->id].lastNicknameChangeTime = currentEpochTime;
                                }
                                break;
                            }
                        case MG_CHAT:
                            {
                                auto currentEpochTime = std::time(0);
                                std::stringstream ss;
                                Message msg;
                                if (HostKeys->isServer && (currentEpochTime - this->connectedPlayers[pi->id].lastMessageTime > MIN_CHAT_DELAY)) {
                                    if (gameDataTrueLength > 256) {
                                        std::memcpy(msg.lastChatMessage, gameData, 255);
                                        msg.lastChatMessage[255] = '\0';
                                    } else {
                                        std::memcpy(msg.lastChatMessage, gameData, gameDataTrueLength);
                                        msg.lastChatMessage[gameDataTrueLength - 1] = '\0';
                                    }
                                    msg.lastChatMessageSource = pi->id;

                                    {
                                        cereal::JSONOutputArchive oarchive(ss);
                                        oarchive(msg);
                                    }
                                    for (auto it : this->connectedPeers) {
                                        this->FormatAndSend<char>(MG_CHAT, it.second, ss.str().length() + 1, ss.str().data());
                                        //this->FormatAndSend(MG_CHAT, it.second, ss.str().length() + 1, reinterpret_cast<unsigned char *>(ss.str().data()));
                                    }
                                    this->connectedPlayers[pi->id].lastMessageTime = currentEpochTime;
                                } else if (!HostKeys->isServer) {
                                    ss << gameData;
                                    {
                                        cereal::JSONInputArchive iarchive(ss);
                                        iarchive(msg);
                                    }
                                    this->Messages.push_back(msg);
                                }
                                break;
                            }
                    }
                    delete[] gameData;
                }

                destroy_packet:
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                if (!this->HostKeys->isServer && this->GetPeersVector().size() == 0) { // handles client connection timeout
                    //std::cout << "retrysafe" << std::endl;
                    this->retrySafe = true;
                    break;
                }
                // Remove peer from known list
                this->connectedPeers.erase(static_cast<PeerInformation*>(event.peer->data)->id);
                // Remove player from known list
                this->connectedPlayers.erase(static_cast<PeerInformation*>(event.peer->data)->id);
                // Server-only: Let everyone know
                sendPlayers();

                // NOTE: only event.peer->data is valid! everything else is invalid in the peer.
                std::stringstream disconnectMessage;
                char friendlyHostIp[64];

                enet_address_get_host_ip(&event.peer->address, &friendlyHostIp[0], 64);
                disconnectMessage << "Peer disconnecting: " << std::string(friendlyHostIp) << std::endl;
                TraceLog(LOG_INFO, disconnectMessage.str().c_str());
                delete(static_cast<PeerInformation*>(event.peer->data));

                event.peer->data = NULL;
                enet_peer_reset(event.peer);
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

const std::unordered_map<uint32_t, ENetPeer*> Stinky::Host::GetPeersMap() {
    return this->connectedPeers;
}

const std::vector<ENetPeer*> Stinky::Host::GetPeersVector() {
    std::vector<ENetPeer*> vec;
    for (auto it : this->GetPeersMap()) {
        vec.push_back(it.second);
    }
    return vec;
}

ENetPeer * Stinky::Host::FindPeerFromId(uint32_t id) {
    return this->connectedPeers[id];
}

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

Stinky::Host::Host() {
    if (!this->InitializeEnetAndCrypto()) {
        throw std::runtime_error("A subsystem failed to start!");
    }
}

Stinky::Host::~Host() {
    for (unsigned i = 0; i < this->host->connectedPeers; ++i) {
        auto peer = &this->host->peers[i];
        enet_peer_disconnect_now(peer, 0);
        delete(static_cast<PeerInformation*>(peer->data));
    }
    delete(this->HostKeys);
    enet_host_destroy(this->host);
}

Stinky::Server::Server(ENetAddress address, enet_uint8 clients, enet_uint8 channels, enet_uint32 bandwidth) : Stinky::Host::Host() {
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

Stinky::Client::Client(ENetAddress * serverAddress, enet_uint8 outgoing, enet_uint8 channels, enet_uint32 bandwidth) : Stinky::Host::Host() {
    this->HostKeys->isServer = false;
    this->server = nullptr;
    this->host = enet_host_create(NULL, outgoing, channels, bandwidth, bandwidth);

    if (host == NULL) {
        TraceLog(LOG_ERROR, "ENet client failed to start! Bailing out...");
        throw std::runtime_error("Client constructor failed! Check logs.");
    }

    this->serverAddress = serverAddress;
}

void Stinky::Client::AttemptConnect() {
    if (this->retrySafe == true) {
        retrySafe = false;
        TraceLog(LOG_INFO, "Starting new connection...");
        this->server = enet_host_connect(this->host, this->serverAddress, this->host->channelLimit, 0);
    }
}

const std::vector<Stinky::Host::Message> Stinky::Host::GetMessageVector() {
    return this->Messages;
}
