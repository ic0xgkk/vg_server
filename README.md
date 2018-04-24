# Vehicle God Server帮助文档

[![Twitter](https://img.shields.io/badge/Twitter-@Ice_Cream-blue.svg?style=flat)](http://twitter.com/ic0xgkk)
[![Release](https://img.shields.io/github/release/ic0xgkk/vg_server.svg?style=flat)](https://github.com/ic0xgkk/vg_server)

### 特别注意

**它仅仅支持Linux平台！**

整个Vehicle God Server源码只有一个server.c文件，编译仅仅需要

```
gcc server.c [-o outfile] -lpthread 
```
然后你仅仅需要在Linux Terminal执行它二进制文件即可！

写得很low（实用就好了），如有bug请开issue，谢谢！

### 报文格式

数据流结构： 

| ORI | CID | DID | DATA_FRAME |
| ------ | ------ | ------ | ------ |

**其中，**

 - ORI为8bit（1byte）的数据流，为方向标志位，用于判断传入的数据的转发方向，是记录到数据库中还是转发到客户端（全局方向）;
 - CID为8bit（1byte）的数据流，用于解析转发数据流的目标设备地址（即匹配device表中的记录值）;
 - DID为8bit（1byte）的数据流，为数据类型报文，用于判断数据类型以便进行解析和记录;
 - DATA_FRAME为载荷数据段，其中包含参数类信息流，长度为（BUFFER_SIZE-1-1），其中的BUFFER_SIZE可以在宏定义片段中看到;

***包为定长包，包长度为宏定义的大小（BUFFER_SIZE），切勿让数据流超出这个长度，否则会被截断，此程序并未包含TCP包排序功能！！！***

***系统的MTU（最大传输单元）也需要进行调整，以超出BUFFER_SIZE的数值32byte为佳！！！***

### ORI标志位

ORI为方向选择，暂定的标志有如下：

 - ***0x01***  常规转发
 - ***0xB1***  转发方向为补给站

无法匹配ORI标志位将会导致这个接收到的数据被丢弃！

### CID标志位

CID为Car ID，即为车号，设备表文件名为device，如IP地址有变动需要根据device中的格式进行修改，此处的IP地址不是字节序。

**device表每行的格式如下：**

| IP地址 | 半角空格 | CID | 换行符 |
| ------ | ------ | ------ | ------ |

目前已经写入的设备对应关系有：

| IP地址 | CID | 备注 |
| ------ | ------ | ------ |
| 192.168.1.2 | 1 | 步兵1 |
| 192.168.1.3 | 2 | 步兵2 |
| 192.168.1.10 | 50 | 补给站 |

特别需要注意的是，路由器端需要把这些设备的IP地址进行静态分配！或者直接关闭DHCP Server使用静态地址也比较好，保险。

对于OpenWRT/LEDE的话，在  ***网络 > DHCP/DNS > HOSTS和解析文件***  中，可以直接指定DHCP ***租约文件*** 的位置，默认是 ***/tmp/dhcp.leases*** ，可以将其修改到 ***/etc/dhcp.leases*** 以实现重启后DHCP分配的地址不会改变。

### DID标志位

DID为Data format ID,即数据格式号，用于区分不同的数据段的目的，以实现分类处理和队列决策，为后续的自动采样和自动调参做准备


**目前规定的DID对应关系有：**

在此，我们定义C为Car（车）端，定义A为Android客户端，定义S为Server端

| DID | 目的 |
| ------ | ------ |
| 0x01 | C->S 持续发送：对4个轮子的状态信息进行统计并且转发到CID指定的Android设备 |
| 0x02 | S->C 请求：向C请求4个轮子的PID参数 |
| 0x03 | C->S 发送：向S发送4个轮子的PID参数 |
| 0x04 | C->S 请求：向C请求 |
| 0xA1 | A->S 请求：请求S向C请求4个轮子PID参数 |
| 0xA2 | S->A 发送：S向A发送4个轮子PID参数 |

**以下条件不满足DID匹配规则（将无视DID位）：**

 - CID为50时（即为补给站方向）
 - 敬请期待
