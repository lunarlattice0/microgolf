#pragma once
typedef unsigned char PacketType;
constexpr PacketType MG_ERROR = 0x00; // to be used on transmission faults / badly encrypted messages
constexpr PacketType MG_CHAT = 0x01;
