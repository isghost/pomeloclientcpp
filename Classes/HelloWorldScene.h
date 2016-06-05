#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "pomelo.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
// pomelo
	pc_client_t* client;
	int handler_id;
	static void request_cb(const pc_request_t* req, int rc, const char* resp);
	static void event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);
	static int port;

};

#endif // __HELLOWORLD_SCENE_H__
