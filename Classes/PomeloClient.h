#ifndef __POMELO_CLIENT___H__
#define __POMELO_CLIENT___H__
#include "pomelo.h"
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <map>
struct EventCbData{
    int ev_type;
    void * ex_data;
    std::string arg1;
    std::string arg2;
};
struct RequestCbData{
    int rc;
    std::string resp;
    std::string route;
};
struct NotifyCbData{
    int rc;
    std::string route;
};
#define PC_EV_CONNECTED_NAME "onConnected"
class PomeloClient{
public:
    static PomeloClient * createClient();
    static void initLib(void (*pc_log)(int level, const char* msg, ...),
        void* (*pc_alloc)(size_t len), void (*pc_free)(void* d), const char* platform);
    static void cleanupLib();
private:
    static void eventCb(pc_client_t *client, int ev_type, void* ex_data,
                              const char* arg1, const char* arg2);
    static void requestCb(const pc_request_t * req, int rc, const char * resp);
    static void notifyCb(const pc_notify_t * req, int rc);
public:
    void init();
    void update(float delta);
    // client
    int connect(const char* host, int port, const char* handshake_opts);
    int disconnect();
    void * getExData();
    int getState();
    int getConnQuality();
    void * getTransData();
	void destroy();
    // callback
    int requestWithTimeout(const char* route,const char* msg, void* ex_data,
        int timeout, std::function<void(const char * msg)> cb);
    int notifyWithTimeout(const char* route,const char* msg, void* ex_data,
        int timeout, std::function<void(const char * msg)> cb);
    int registerEventCb(const char * route, std::function<void(const char * msg)> cb);
private:
    pc_client_t * client;
    int eventHandler;
    static std::mutex eventLock;
    std::vector<EventCbData> eventCbQueue;
    std::vector<RequestCbData> requestCbQueue;
    std::vector<NotifyCbData> notifyCbQueue;
    std::map<std::string, std::function<void(const char * msg)>> functionMap;
};
#endif