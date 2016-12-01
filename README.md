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


---

---

关于vs开发cocos2dx3.0使用libpomelo2的一些bug解决
===

## libpomelo2

网易的libpomelo 更新成了 libpomelo2 ,所以下载新的libpomelo2
（注意不要用旧的，会出现问题，libpomelo2 相比libpomelo2有了很大的改变）
下载网址
https://github.com/NetEase/libpomelo2

讨论区
http://nodejs.netease.com/topic/540702e0898634292cd0a588

![](http://i.imgur.com/fYKeHlu.png)

## vs2015 编译libpomelo2

* 建议vs2015 免费的社区版就行了，用vs2013 编译的时候出现 v14.0错误，还有各种未解析的符号错误

* gyp

会有人提示去下载（下面的网址根本是没有东西的，坑）
https://github.com/martine/gyp 


当然libpomelo2给的文档如下（要知道是需要翻墙的，网易能自己fork后,标注版权再给个网址吗，坑）
 
```
git clone https://chromium.googlesource.com/external/gyp
cd gyp
python setup.py install
```

我是翻墙下载的，如下
![](http://i.imgur.com/Rex91Ct.jpg)

可以将文件拷贝到libpomel2目录下

![](http://i.imgur.com/BBNIGpx.png)


## 编译生成.lib文件

还是使用vs2015

使用的命令
```
gyp.bat --depth=.  ../pomelo.gyp -Dlibrary=static_library -DTO=pc​​
```
或改进如下
```
gyp.bat --depth=.  ../pomelo.gyp -Duse_sys_openssl=false -Dlibrary=static_library -DTO=pc
```
会出现 sln
![](http://i.imgur.com/6n9c2I2.png)


随后vs2015打开对象目进行重新生成，会生成如下的三个lib(有错误，自己google)
![](http://i.imgur.com/K8YL0fP.png)


参考如下网址
http://blog.sina.com.cn/s/blog_537283200102w0ca.html


# cocos2dx 客户端项目


#### cocos2dx环境

* ccocos2dx v3.10
* vs2015 社区版

#### 

* fork 如下地址的项目 
https://github.com/doctording/pomeloclientcpp

* 自己新建立的项目，引入库文件，头文件（vs如何引入，自己google）
![](http://i.imgur.com/qoEhRxV.png)

* copy原项目的Classes 和 Resource 到自己的项目中，编译会可能会出现如下的问题
![](http://i.imgur.com/ZbQqYyK.png)


解决办法是在PomeloClient.h中添加如下#paragma comment()

````
#ifndef __POMELO_CLIENT___H__
#define __POMELO_CLIENT___H__
#include "pomelo.h"
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <map>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libuv.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Psapi.lib")
````

这里参考如下
http://nodejs.netease.com/topic/52ba9e68b5bc7de2050b3455


上面的步骤都没问题的话，客户端应该可以正常运行，
在clone 官方的chatofpomelo-websocket 例子
https://github.com/NetEase/chatofpomelo-websocket.git

运行服务端，注意node版本不能太低
我的服务端环境是
* ubuntu14.04 32bit
* node5.8.0

# 运行截图
![](http://i.imgur.com/0AkG4D0.png)

![](http://i.imgur.com/JxgwQsr.png)

![](http://i.imgur.com/WLK0MkV.png)