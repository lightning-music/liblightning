#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include "types.h"

/**
 * lightning server data type
 *
 * The server initializes a jack client and exposes
 * OSC endpoints to load and playback audio samples.
 *
 * The server also broadcasts messages over OSC
 * that provide clients a way to know what it is doing
 * internally.
 */
typedef struct LightningServer *LightningServer;

/**
 * Create a new lightning server.
 * If @a listenPort is NULL then liblo chooses an unused system port.
 */
LightningServer
LightningServer_init(const char *listenPort,
                     const char *broadcastPort,
                     const char *broadcastHost,
                     int num_dirs,
                     char **search_dirs);

/**
 * Create a new lightning server that is part of a multicast group.
 * See http://tldp.org/HOWTO/Multicast-HOWTO-2.html for an
 * explanation of multicast.
 */
LightningServer
LightningServer_multicast(const char *group,
                          const char *listenPort,
                          const char *broadcastPort,
                          const char *broadcastHost);

/**
 * Return the listening port for a lightning server.
 */
int
LightningServer_listen_port(LightningServer server);

/**
 * Return the broadcast port for a lightning server.
 */
int
LightningServer_broadcast_port(LightningServer server);

/**
 * Have the server start listening for messages.
 * This blocks forever.
 */
void
LightningServer_listen(LightningServer server);

/**
 * Play a sample
 */
int
LightningServer_play_sample(LightningServer server,
                            const char *file,
                            pitch_t pitch,
                            gain_t gain);

/**
 * Free the resources held by a lightning server
 */
void
LightningServer_free(LightningServer *server);

#endif
