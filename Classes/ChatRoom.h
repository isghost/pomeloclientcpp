#ifndef __CHATROOM_SCENE_H__
#define __CHATROOM_SCENE_H__

#include "cocos2d.h"
#include "pomelo.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include <vector>
#include <mutex>
struct EventCb{
	pc_client_t* client;
	int ev_type;
	void* ex_data;
	std::string arg1;
	std::string arg2;
};
class ChatRoom : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(int port,std::string name, std::string roomId);

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ChatRoom);
// pomelo
	pc_client_t* client;
	int handler_id;
	std::string roleName;
	std::string roomId;
	static void request_cb(const pc_request_t* req, int rc, const char* resp);
	static void event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2);
	void connectToServer(int port);
	void addMsg(std::string router, std::string msg);
	cocos2d::ui::ListView * listView;
	std::vector<EventCb> eventCbArray;
	std::mutex eventMutex;
	void update(float delta);

};

#endif // __ChatRoom_SCENE_H__
