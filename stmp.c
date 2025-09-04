/*  stmp.c - SplaT Middleware Protocol C library implementation file
    Copyright (C) 2025 splatte.dev

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

#include "stmp.h"

#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

/**
 * stmp_init_result - initializes stmp_result struct to default values
 * @result - the result structure to initialize
 */
void stmp_init_result(struct stmp_result *result)
{
    result->size = 0;
    result->error = STMP_ERR_NONE;
}

/**
 * stmp_init_packet - initializes stmp_packet struct to default values
 * @packet - the packet structure to initialize
 */
void stmp_init_packet(struct stmp_packet *packet)
{
    packet->version = 0;
    packet->type = 0;
    packet->arg = 0;
    packet->flags = 0;

    packet->payload = NULL;
}

/**
 * stmp_serialize - serialize a stmp_packet into a buffer
 * @buffer - the buffer the struct will be serialized into
 * @size - the size of the buffer
 * @packet - the packet to serialize from
 * @result - the result of the serialization
 */
void stmp_serialize(uint8_t *buffer, size_t size, struct stmp_packet *packet, struct stmp_result *result)
{
    int packetTerminateIndex = STMP_PACKET_HEADER_SIZE;

    stmp_init_result(result);
    bzero(buffer, size);

    if (size < STMP_PACKET_MIN_SIZE || size > STMP_PACKET_MAX_SIZE) {
        result->error = STMP_ERR_BAD_SIZE;
        return;
    }

    int version = 0;
    for (int i = 0; i < sizeof(stmp_versions) / sizeof(stmp_versions[0]); i++) {
        if (packet->version == stmp_versions[i]) {
            version = stmp_versions[i];
            break;
        }
    }

    if (version == 0) {
        result->error = STMP_ERR_BAD_VERSION;
        return;
    }

    if (packet->type < STMP_TYPE_INIT || packet->type > STMP_TYPE_INVALID) {
        result->error = STMP_ERR_BAD_TYPE;
        return;
    }


    switch (packet->type) {
        case STMP_TYPE_INIT:
            if (packet->arg < STMP_ARG_INIT_INIT || packet->arg > STMP_ARG_INIT_ACCEPT) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_PING:
            if (packet->arg != STMP_ARG_PING) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_SEND:
            if (packet->arg != STMP_ARG_SEND) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_TERM:
            if (packet->arg < STMP_ARG_TERM_CLEAN || packet->arg > STMP_ARG_TERM_BUSY) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_INVALID:
            if (packet->arg < STMP_ARG_INVALID_VERSION || packet->arg > STMP_ARG_INVALID_PAYLOAD) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
    }

    if ((packet->type == STMP_TYPE_INVALID || packet->type == STMP_TYPE_INIT)
        && packet->payload != STMP_PAYLOAD_EMPTY) {
        result->error = STMP_ERR_BAD_PAYLOAD;
        return;
    }

    buffer[0] = version;
    buffer[1] = packet->type;
    buffer[2] = packet->arg;
    buffer[3] = packet->flags;

    for (int i = 0; i < STMP_PACKET_MAX_SIZE - STMP_PACKET_HEADER_SIZE - i; i++) {
        if (packet->payload == STMP_PAYLOAD_EMPTY) {
            packetTerminateIndex = STMP_PACKET_HEADER_SIZE + i + 1;
            break;
        }

        buffer[STMP_PACKET_HEADER_SIZE + i] = packet->payload[i];
        if (packet->payload[i] == '\0') {
            packetTerminateIndex = STMP_PACKET_HEADER_SIZE + i + 1;
            break;
        }
    }

    buffer[packetTerminateIndex] = STMP_PACKET_TERMINATE;

    result->size = packetTerminateIndex;
    result->error = STMP_ERR_NONE;
}

/**
 * stmp_deserialize - deserialize a buffer into a stmp_packet
 * @buffer - the buffer to deserialize from
 * @size - the size of the buffer
 * @packet - the packet to deserialize into
 * @result - the result of the serialization
 */
void stmp_deserialize(uint8_t *buffer, size_t size, struct stmp_packet *packet, struct stmp_result *result)
{
    int payloadSize = 0;
    int packetTerminateIndex = STMP_PACKET_HEADER_SIZE;

    stmp_init_packet(packet);
    bzero(packet, sizeof(struct stmp_packet));

    if (size < STMP_PACKET_MIN_SIZE || size > STMP_PACKET_MAX_SIZE) {
        result->error = STMP_ERR_BAD_SIZE;
        return;
    }

    int version = 0;
    for (int i = 0; i < sizeof(stmp_versions) / sizeof(stmp_versions[0]); i++) {
        if (buffer[0] == stmp_versions[i]) {
            version = stmp_versions[i];
            break;
        }
    }

    if (version == 0) {
        result->error = STMP_ERR_BAD_VERSION;
        return;
    }

    if (buffer[1] < STMP_TYPE_INIT && buffer[1] > STMP_TYPE_INVALID) {
        result->error = STMP_ERR_BAD_TYPE;
        return;
    }

    switch (packet->type) {
        case STMP_TYPE_INIT:
            if (buffer[2] < STMP_ARG_INIT_INIT || buffer[2] > STMP_ARG_INIT_ACCEPT) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_PING:
            if (buffer[2] != STMP_ARG_PING) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_SEND:
            if (buffer[2] != STMP_ARG_SEND) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_TERM:
            if (buffer[2] < STMP_ARG_TERM_CLEAN || buffer[2] > STMP_ARG_TERM_BUSY) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
        case STMP_TYPE_INVALID:
            if (buffer[2] < STMP_ARG_INVALID_VERSION || buffer[2] > STMP_ARG_INVALID_PAYLOAD) {
                result->error = STMP_ERR_BAD_ARG;
                return;
            }
            break;
    }

    packet->version = version;
    packet->type = buffer[1];
    packet->arg = buffer[2];
    packet->flags = buffer[3];

    if ((buffer[1] == STMP_TYPE_INVALID || buffer[1] == STMP_TYPE_INIT)
        && buffer[STMP_PACKET_HEADER_SIZE] != STMP_PAYLOAD_EMPTY) {
        result->error = STMP_ERR_BAD_PAYLOAD;
        return;
    }

    char *payload = malloc(sizeof(char) * (STMP_PACKET_MAX_SIZE - STMP_PACKET_HEADER_SIZE));
    if (payload == NULL) {
        result->error = STMP_ERR_BAD_MALLOC;
        return;
    }

    for (int i = 0; i < STMP_PACKET_MAX_SIZE - STMP_PACKET_HEADER_SIZE - i; i++) {
        payload[i] = buffer[STMP_PACKET_HEADER_SIZE + i];
        payloadSize++;
        if (buffer[STMP_PACKET_HEADER_SIZE + i] == '\0') {
            break;
        }
    }

    void *r = realloc(payload, payloadSize);
    if (r == NULL) {
        result->error = STMP_ERR_BAD_MALLOC;
        free(payload);
        return;
    }

    payload = r;

    packet->payload = payload;
    packetTerminateIndex = STMP_PACKET_HEADER_SIZE + payloadSize;

    if (buffer[packetTerminateIndex] != STMP_PACKET_TERMINATE) {
        result->error = STMP_ERR_BAD_TERMINATE;
        free(payload);
        return;
    }

    result->size = packetTerminateIndex;
    result->error = STMP_ERR_NONE;
}

// int main (void)
// {
//     // char *test = "hello\0";
//     struct stmp_packet packet = {
//         .version = 1,
//         .type = STMP_TYPE_PING,
//         .arg = STMP_ARG_PING,
//         .flags = STMP_FLAGS_NONE,
//         .payload = STMP_PAYLOAD_EMPTY
//     };
//
//     struct stmp_result result = {0};
//
//     uint8_t buffer[20];
//
//     stmp_serialize(buffer, sizeof(buffer), &packet, &result);
//
//     if (result.error != STMP_ERR_NONE) {
//         printf("Hit an error: %d\n", result.error);
//         return 1;
//     }
//
//     for (int i = 0; i < sizeof(buffer); i++) {
//         printf("%d\n", buffer[i]);
//     }
//
//     stmp_deserialize(buffer, sizeof(buffer), &packet, &result);
//     printf("result error: %d\n", result.error);
//
//     printf("packet version after deserialize: %d\n", packet.version);
//     printf("packet type after deserialize: %d\n", packet.type);
//     printf("packet arg after deserialize: %d\n", packet.arg);
//     printf("packet flags after deserialize: %d\n", packet.flags);
//     printf("packet payload after deserialize: %s\n", packet.payload);
//
//     return 0;
// }
