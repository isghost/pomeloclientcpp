#include "ChatRoom.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "json\document.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define flag ((void *)0x12)
#define sendFlag ((void *)0x13)


Scene* ChatRoom::createScene(int port, std::string name, std::string roomId)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = ChatRoom::create();
	layer->scheduleUpdate();
	layer->roleName = name;
	layer->roomId = roomId;
	layer->connectToServer(port);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool ChatRoom::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(ChatRoom::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "ChatRoom" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
//----------------------------------------- 
	auto name = Label::createWithTTF("msg:", "fonts/Marker Felt.ttf", 16.0f);
	name->setPosition(visibleSize.width / 2 - 100, 100);
	this->addChild(name,10);

	auto editBoxSize = Size(200, 30);
	auto nameEditBox = ui::EditBox::create(editBoxSize, "orange_edit.png");
	nameEditBox->setPosition(Vec2(visibleSize.width / 2 + 50, 120));
	this->addChild(nameEditBox);

	// Create the list view ex
	listView = ListView::create();
	// set list view ex direction
	listView->setDirection(ui::ScrollView::Direction::VERTICAL);
	listView->setBounceEnabled(true);
	listView->setBackGroundImage("green_edit.png");
	listView->setBackGroundImageScale9Enabled(true);
	listView->setContentSize(Size(240, 300));
	listView->setPosition(Vec2(40, 150));
	//listView->setScrollBarPositionFromCorner(Vec2(7, 7));
	this->addChild(listView,111);

	auto loginBtn = ui::Button::create("animationbuttonnormal.png",
		"animationbuttonpressed.png");
	loginBtn->setTitleText("entry");
	loginBtn->setAnchorPoint(Vec2(0.5, 0.5));
	loginBtn->setPosition(Vec2(visibleSize.width / 2 , 50));
	loginBtn->addClickEventListener([=](Ref*){
		auto str1 = nameEditBox->getText();
		char str[200];
		sprintf(str, "{\"rid\": \"2\",\"content\":\" %s\",\"from\":\"ccy\",\"target\":\"*\"}", str1);
		pc_request_with_timeout(client, "chat.chatHandler.send", str, sendFlag, 2, request_cb);
	});
	this->addChild(loginBtn, 11);


    return true;
}


void ChatRoom::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void ChatRoom::request_cb(const pc_request_t* req, int rc, const char* resp)
{
	CCLOG("request callback = %s ", resp);
}

void ChatRoom::event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2){
	auto chatLayer = (ChatRoom *)ex_data;
	if (ev_type == PC_EV_CONNECTED){
		char reqMsg[200];
		sprintf(reqMsg, "{\"username\": \"%s\",\"rid\":\"%s\"}", chatLayer->roleName.c_str(), chatLayer->roomId.c_str());
		pc_request_with_timeout(client, "connector.entryHandler.enter", reqMsg, (void *)0x14, 2, request_cb);
	}
	else if (ev_type == PC_EV_USER_DEFINED_PUSH){
		// 回调不在主线程，需要先放入队列，在主线程update的时候
		chatLayer->eventMutex.lock();
		chatLayer->eventCbArray.push_back(EventCb{ client, ev_type, ex_data, arg1, arg2 });
		chatLayer->eventMutex.unlock();
		CCLOG("push router = %s, push msg = %s", arg1, arg2);
	}
	CCLOG("event %d =", ev_type);

}

void ChatRoom::connectToServer(int port){
	// pomelo init
	pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	client = (pc_client_t*)malloc(pc_client_size());
	pc_client_init(client, (void*)0x11, &config);
	handler_id = pc_client_add_ev_handler(client, event_cb, (void *)this, NULL);
	// IP需要替换成服务器所在地址，这里写死掉了
	pc_client_connect(client, "192.168.0.14", port, NULL);
}

void ChatRoom::addMsg(std::string router, std::string msg){
	std::string showStr = "error " + router;
	using rapidjson::Document;
	Document doc;
	doc.Parse<0>(msg.c_str());
	if (router == "onChat"){
		std::string chatMsg = doc["msg"].GetString();
		std::string from = doc["from"].GetString();
		std::string target = doc["target"].GetString();
		showStr = from + " to " + target + " : " + chatMsg;
	}
	else if (router == "onAdd"){
		std::string user = doc["user"].GetString();
		showStr = "user " + user + " enter the room";
	}
	else if (router == "onLeave"){
		std::string user = doc["user"].GetString();
		showStr = "user " + user + " leave the room";
	}
	auto label = Text::create(showStr, "Arial", 15);
	label->setContentSize(Size(200, 18));
	listView->insertCustomItem(label,0);
}

void ChatRoom::update(float delta){
	if (eventCbArray.size() > 0){
		eventMutex.lock();
		for (int i = 0; i < eventCbArray.size(); i++){
			pc_client_t* client = eventCbArray[i].client;
			int ev_type = eventCbArray[i].ev_type;
			void* ex_data = eventCbArray[i].ex_data;
			std::string arg1 = eventCbArray[i].arg1;
			std::string arg2 = eventCbArray[i].arg2;
			CCLOG("chat room %s", arg2);
			addMsg(arg1, arg2);
		}
		eventCbArray.clear();
		eventMutex.unlock();
	}
}