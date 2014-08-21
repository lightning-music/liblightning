#ifndef WEB_SERVER_H_INCLUDED
#define WEB_SERVER_H_INCLUDED

typedef struct WebServer *WebServer;

/**
 * Initialize a new web server.
 *
 * @param  {int}            port - Listening port
 */
WebServer
WebServer_init(int port);

/**
 * Blocking listen.
 *
 * @param  {WebServer}      ws - web server
 */
void
WebServer_listen(WebServer ws);

/**
 * Free a web server.
 *
 * @param  {WebServer *}    ws - web server pointer
 */
void
WebServer_free(WebServer *ws);

#endif // WEB_SERVER_H_INCLUDED
