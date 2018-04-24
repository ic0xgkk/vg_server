# Vehicle God Server帮助文档

[![Twitter](https://img.shields.io/badge/Twitter-@Ice_Cream-blue.svg?style=flat)](http://twitter.com/ic0xgkk)
[![Release](https://img.shields.io/github/release/ic0xgkk/vg_server.svg?style=flat)](https://github.com/ic0xgkk/vg_server)

### 特别注意

**它仅仅支持Linux平台！**

**没错，你没有看错，整个Vehicle God Server源码只有一个server.c文件，编译仅仅需要**

```
gcc server.c [-o outfile] -lpthread 
```
**然后你仅仅需要在Linux Terminal执行它二进制文件即可！**

**写得很low（实用就好了），如有bug请开issue，谢谢！**

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



