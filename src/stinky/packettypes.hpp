#pragma once
typedef unsigned char PacketType;
constexpr PacketType MG_ERROR = 0x00; // to be used on transmission faults / badly encrypted messages
constexpr PacketType MG_LUA_DATA = 0x01; // not in use
constexpr PacketType MG_PLAYERLIST = 0x02; // List of Players; sent from server to clients
constexpr PacketType MG_WHOAMI = 0x03; // Server-only: notify clients of their own "player", Client-Only: store own id.
constexpr PacketType MG_NICKNAME_CHANGE = 0x04; // Nickname change request from client to server
constexpr PacketType MG_CHAT = 0x05; // Server: relay messages from client; client: send message to client
//constexpr PacketType MG_TEST = 0xFF; // disable in prod
