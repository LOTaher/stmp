/*  stmp.h - SplaT Middleware Protocol C library definition file
    Copyright (C) 2024 splatte.dev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdint.h>
#include <stdlib.h>


/* [0] Version */
static const int stmp_versions[] = {1};

/* [1] Type */
#define STMP_TYPE_INIT 0x01
#define STMP_TYPE_PING 0x02
#define STMP_TYPE_SEND 0x03
#define STMP_TYPE_TERM 0x04
#define STMP_TYPE_INVALID 0x05

/* [2] Argument */
#define STMP_ARG_INIT_INIT 0x01
#define STMP_ARG_INIT_ACCEPT 0x02

#define STMP_ARG_PING 0x00

#define STMP_ARG_SEND 0x00

#define STMP_ARG_TERM_CLEAN 0x01
#define STMP_ARG_TERM_BUSY 0x02

#define STMP_ARG_INVALID_VERSION 0x01
#define STMP_ARG_INVALID_TYPE 0x02
#define STMP_ARG_INVALID_MESSAGE 0x03
#define STMP_ARG_INVALID_ARGUMENT 0x04
#define STMP_ARG_INVALID_FLAGS 0x05
#define STMP_ARG_INVALID_PAYLOAD 0x06

/* [3] Flags */
typedef uint8_t stmp_flag;
#define STMP_FLAGS_NONE 0
#define STMP_FLAGS_LOG (1 << 0)
#define STMP_FLAGS_INCOGNITO (1 << 1)

/* [4] Payload */
#define STMP_PAYLOAD_EMPTY 0x00

/* Packet */
#define STMP_PACKET_HEADER_SIZE 0x04
#define STMP_PACKET_MAX_SIZE 0x5DC
#define STMP_PACKET_MIN_SIZE 0x05
#define STMP_PACKET_TERMINATE 0x7F

typedef uint8_t stmp_version;
typedef uint8_t stmp_type;
typedef uint8_t stmp_arg;

enum stmp_error {
    STMP_ERR_NONE,
    STMP_ERR_BAD_SIZE,
    STMP_ERR_BAD_VERSION,
    STMP_ERR_BAD_TYPE,
    STMP_ERR_BAD_ARG,
    STMP_ERR_BAD_PAYLOAD,
    STMP_ERR_BAD_MALLOC,
    STMP_ERR_BAD_REALLOC,
    STMP_ERR_BAD_TERMINATE
};

struct stmp_packet {
    stmp_version version;
    stmp_type type;
    stmp_arg arg;
    stmp_flag flags;

    char *payload;
};

struct stmp_result {
    int size;
    enum stmp_error error;
};

void stmp_serialize(uint8_t *buffer, size_t size, struct stmp_packet *packet, struct stmp_result *result);
void stmp_deserialize(uint8_t *buffer, size_t size, struct stmp_packet *packet, struct stmp_result *result);
void stmp_init_result(struct stmp_result *result);
void stmp_init_packet(struct stmp_packet *packet);
