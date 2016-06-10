#include "PomeloClient.h"
using namespace std;
mutex  PomeloClient::eventLock;

PomeloClient * PomeloClient::createClient(){
    auto client = new PomeloClient();
    client->init();
    return client;
}

void PomeloClient::initLib(void (*pc_log)(int level, const char* msg, ...),
        void* (*pc_alloc)(size_t len), void (*pc_free)(void* d), const char* platform){
    pc_lib_init(pc_log, pc_alloc, pc_free, platform);
}
void PomeloClient::cleanupLib(){
    pc_lib_cleanup();
}

void PomeloClient::eventCb(pc_client_t *client, int ev_type, void* ex_data,
                              const char* arg1, const char* arg2){
    auto pomeloClient = (PomeloClient *)pc_client_ex_data(client);
	if (arg1 == NULL){
		arg1 = "";
	}
	if (arg2 == NULL){
		arg2 = "";
	}
    eventLock.lock();
	EventCbData t = { ev_type, ex_data, arg1, arg2 };
	pomeloClient->eventCbQueue.push_back(t);
    eventLock.unlock();
}

void PomeloClient::requestCb(const pc_request_t * req, int rc, const char * resp){
    auto pomeloClient = (PomeloClient *)pc_client_ex_data(pc_request_client(req));
    auto route = pc_request_route(req);
    eventLock.lock();
	pomeloClient->requestCbQueue.push_back(RequestCbData{ rc, resp, route });
    eventLock.unlock();
}

void PomeloClient::notifyCb(const pc_notify_t * req, int rc){
	auto pomeloClient = (PomeloClient *)pc_client_ex_data(pc_notify_client(req));
    auto route = pc_notify_route(req);
    eventLock.lock();
	pomeloClient->notifyCbQueue.push_back(NotifyCbData{ rc, route });
    eventLock.unlock();
}

void PomeloClient::init(){
	pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	client = (pc_client_t *)malloc(pc_client_size());
	pc_client_init(client, (void*)this, &config);
	eventHandler = pc_client_add_ev_handler(client, PomeloClient::eventCb, (void *)this, NULL);
}
void PomeloClient::update(float delta){
    if(eventCbQueue.size() <= 0 && requestCbQueue.size() <= 0 && notifyCbQueue.size() <= 0){
        return ;
    }
    eventLock.lock();
    for(int i=0;i<eventCbQueue.size();i++){
        auto eventCbData = eventCbQueue[i];
        if(eventCbData.ev_type == PC_EV_USER_DEFINED_PUSH){
            functionMap[eventCbData.arg1](eventCbData.arg2.c_str());
        }
		else if (eventCbData.ev_type == PC_EV_CONNECTED){
			functionMap[PC_EV_CONNECTED_NAME]("");
		}
    }
    for(int i=0;i<requestCbQueue.size();i++){
        auto requestCbData = requestCbQueue[i];
        if(requestCbData.rc == PC_RC_OK){
            functionMap[requestCbData.route](requestCbData.resp.c_str());
        }
    }
    for(int i=0;i<notifyCbQueue.size();i++){
        auto notifyCbData = notifyCbQueue[i];
        if(notifyCbData.rc == PC_RC_OK){
			functionMap[notifyCbData.route]("{}");
        }
    }
	eventCbQueue.clear();
	requestCbQueue.clear();
	notifyCbQueue.clear();
    eventLock.unlock();
}
// client
int PomeloClient::connect(const char* host, int port, const char* handshake_opts){
    return pc_client_connect(client, host, port, handshake_opts);
}
int PomeloClient::disconnect(){
    return pc_client_disconnect(client);
}
void * PomeloClient::getExData(){
    return pc_client_ex_data(client);
}
int PomeloClient::getState(){
    return pc_client_state(client);
}
int PomeloClient::getConnQuality(){
    return pc_client_conn_quality(client);
}
void * PomeloClient::getTransData(){
    return pc_client_trans_data(client);
}
// callback
int PomeloClient::requestWithTimeout(const char* route,const char* msg, void* ex_data,
    int timeout, std::function<void(const char * msg)> cb){
	functionMap.erase(route);
	functionMap[route] = cb;
    return pc_request_with_timeout(client, route, msg, ex_data, timeout, PomeloClient::requestCb);
}
int PomeloClient::notifyWithTimeout(const char* route,const char* msg, void* ex_data,
    int timeout, std::function<void(const char * msg)> cb){
	functionMap.erase(route);
	functionMap[route] = cb;
    return pc_notify_with_timeout(client, route, msg, ex_data, timeout, PomeloClient::notifyCb);
}
int PomeloClient::registerEventCb(const char * route, std::function<void(const char * msg)> cb){
    functionMap.erase(route);
    functionMap[route] = cb;
	return 0;
}

void PomeloClient::destroy(){
	pc_client_disconnect(client);
	pc_client_rm_ev_handler(client, eventHandler);
	pc_client_cleanup(client);
	free(client);
	delete this;
}