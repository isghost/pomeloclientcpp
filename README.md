# pomeloclientcpp
pomelo的cocos2dx客户端的C++版本，使用新版的libpomelo2    
cocos2dx 版本为v3.10    
客户端平台： win7    
服务端系统： ubuntu 14.01 (在win7配置麻烦)    

# 运行方式
## 服务端
采用的是[官方教程](https://github.com/NetEase/pomelo/wiki/chat%E6%BA%90%E7%A0%81%E4%B8%8B%E8%BD%BD%E4%B8%8E%E5%AE%89%E8%A3%85)里的例子。

    $ git clone https://github.com/NetEase/chatofpomelo-websocket.git
    $ git checkout tutorial-starter

## 客户端
1. 根据[libpomelo2](https://github.com/NetEase/libpomelo2)生成```libpomelo2.lib```，```libuv.lib```，```openssl.lib```这三个静态库，将其导入到
cocos2dx工程，这三个文件放在了```Classes/lib```，头文件在```Classes/lib/include```。静态库如何导入，详情百度，或根据自己需要导入工程。

2. 新建cocos2dx C++工程，替换 Classes和Resources这两个文件夹

3. 修改客户端代码中写死的IP地址，将```192.168.0.14```替换成服务端实际所在的IP

然后可以编译运行。
