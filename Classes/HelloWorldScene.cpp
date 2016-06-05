#include "HelloWorldScene.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "ui/UIButton.h"
#include "json\document.h"
#include "ChatRoom.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define flag ((void *)0x11)

int HelloWorld::port = 0;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
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
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
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

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);

	auto name = Label::createWithTTF("name:", "fonts/Marker Felt.ttf", 16.0f);
	name->setPosition(visibleSize.width / 2 - 100, 300);
	this->addChild(name,10);
	auto room = Label::createWithTTF("roomId:", "fonts/Marker Felt.ttf", 16.0f);
	room->setPosition(visibleSize.width / 2 - 100 , 200);
	this->addChild(room,10);
	auto editBoxSize = Size(200, 30);
	auto nameEditBox = ui::EditBox::create(editBoxSize, "orange_edit.png");
	nameEditBox->setPosition(Vec2(visibleSize.width / 2 + 50, 300));
	this->addChild(nameEditBox);
	auto roomEditBox = ui::EditBox::create(editBoxSize, "orange_edit.png");
	roomEditBox->setPosition(Vec2(visibleSize.width / 2 + 50, 200));
	this->addChild(roomEditBox);

	auto loginBtn = ui::Button::create("animationbuttonnormal.png",
		"animationbuttonpressed.png");
	loginBtn->setTitleText("entry");
	loginBtn->setAnchorPoint(Vec2(0.5, 0.5));
	loginBtn->setPosition(Vec2(visibleSize.width / 2 , 100));
	loginBtn->addClickEventListener([=](Ref*){
		auto name = nameEditBox->getText();
		auto roomId = roomEditBox->getText();
		CCLOG("name = %s, room = %s", name, roomId);
		auto scene = ChatRoom::createScene(port, name, roomId);
		Director::getInstance()->replaceScene(scene);
	});
	this->addChild(loginBtn, 11);
	// pomelo init
	pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	pc_lib_init(NULL, NULL, NULL, NULL);
	client = (pc_client_t*)malloc(pc_client_size());
	pc_client_init(client, (void*)0x11, &config);
	handler_id = pc_client_add_ev_handler(client, event_cb, flag, NULL);
	pc_client_connect(client, "192.168.0.14", 3014, NULL);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::request_cb(const pc_request_t* req, int rc, const char* resp)
{
	printf("%s ", resp);
	using rapidjson::Document;
	Document doc;
	doc.Parse<0>(resp);
	port = doc["port"].GetInt();
	auto client = pc_request_client(req);
	pc_client_disconnect(client);
	pc_client_cleanup(client);
	CCLOG("port = %d", port);
}

void HelloWorld::event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2){
	if (ex_data == flag && ev_type == PC_EV_CONNECTED){
		//简单处理，写死获取connect地址
		pc_request_with_timeout(client, "gate.gateHandler.queryEntry", "{\"username\": \"test\",\"uid\":\"1\"}", (void *)0x14, 2, request_cb);
	}
	printf("event %d =", ev_type);

}