/*  libstmp.c - SplaT Middleware Protocol C API
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

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#include "stmp.h"
#include "libstmp.h"
#include "endpoints.h"

enum libstmp_error libstmp_init_endpoint(struct stmp_endpoint *endpoint)
{
    endpoint->id = -1;
    endpoint->name = NULL;

    endpoint->socketType = -1;
    endpoint->socket = 0;

    endpoint->inPort = 0;
    endpoint->inIP = 0;
    endpoint->unFspath = NULL;

    int networkLock = pthread_mutex_init(&endpoint->lock, NULL);
    if (networkLock != 0) return LIBSTMP_ERR_INIT_MUTEX;

    endpoint->logLevel = 0;

    return LIBSTMP_ERR_NONE;
}

void libstmp_message_send(struct stmp_packet *packet, struct stmp_result *result, struct stmp_endpoint *endpoint)
{
    stmp_init_result(result);

    uint8_t buffer[STMP_PACKET_MAX_SIZE];
    stmp_serialize(buffer, sizeof(buffer), packet, result);
    if (result->error != STMP_ERR_NONE) {
        printf("[libstmp_message_send] Error serializing packet\n");
        return;
    }

    // threads are needed due to SONAR plus any action needed
    pthread_mutex_lock(&endpoint->lock);
    send(endpoint->socket, buffer, sizeof(buffer), 0);
    pthread_mutex_unlock(&endpoint->lock);
}

void libstmp_message_log(struct stmp_endpoint *endpoint, char *log)
{
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);

    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "%s[%d/%d/%d %d:%d] [%s] [%s]: %s%s",
    endpointColors[endpoint->id], localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year + 1900,
    localTime->tm_hour, localTime->tm_min, libstmp_loglevel_string[endpoint->logLevel],
    endpoint->name, log, ANSI_COLOR_RESET);

    printf("%s\n", buffer);
}

int main()
{
    struct stmp_endpoint endpoint = {0};
    libstmp_init_endpoint(&endpoint);

    endpoint.id = HOTEL_EPT_CHECK_IN;
    endpoint.name = "Splatte Hotel";

    char *logMsg = "This is a test log!";

    libstmp_message_log(&endpoint, logMsg);

    return 0;
}
