#include <pthread.h>

/* TCP */
#define SOCKET_IN 0
/* UNIX */
#define SOCKET_UN 1

#define LOG_INFO 0
#define LOG_WARNING 1
#define LOG_ERROR 2

static const char *libstmp_loglevel_string[] = {
    "INFO",
    "WARNING",
    "ERROR"
};

#define ERR_INIT_MUTEX 0

enum libstmp_error {
    LIBSTMP_ERR_NONE,
    LIBSTMP_ERR_INIT_MUTEX
};

/*
* An endpoint in STMP is what SERVER you're sending a message to. This information
* is given to the broker to use and open a connection
*/
struct stmp_endpoint {
    int id;
    char *name;

    int socketType;
    int socket;

    short inPort;
    long inIP;
    char *unFspath;

    pthread_mutex_t lock;

    int logLevel;
};

/**
Only destinations have endpoints.

                    Endpoint
client -> broker -> server

broker forwards message to the specified endpoint
server recieves and sends a response and then a TERM back to broker
connection is closed between broker and server
broker sends response back and TERM to client
connection is closed between broker and client

client <- broker <- server
*/

void libstmp_message_send(struct stmp_packet *packet, struct stmp_result *result, struct stmp_endpoint *endpoint);
void libstmp_message_log(struct stmp_endpoint *endpoint, char *log);
void libstmp_connection_keep_alive(struct stmp_endpoint *endpoint);
void libstmp_connection_terminate();
enum libstmp_error libstmp_init_endpoint(struct stmp_endpoint *endpoint);
