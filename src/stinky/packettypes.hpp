#pragma once
typedef unsigned char PacketType;
constexpr PacketType MG_ERROR = 0x00; // to be used on transmission faults / badly encrypted messages
constexpr PacketType MG_LUA_DATA = 0x01; // lua serialized data. i realize implementing packet handling in C++ only is an ABSOLUTE pain.

constexpr PacketType MG_TEST = 0xFF; // disable in prod
