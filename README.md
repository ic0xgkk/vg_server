# Vehicle God 帮助文档

[![Twitter](https://img.shields.io/badge/Twitter-@Ice_Cream-blue.svg?style=flat)](http://twitter.com/a980883231)
[![Release](https://img.shields.io/github/release/ic0xgkk/vg_server.svg?style=flat)](https://github.com/ic0xgkk/vg_server)

报文格式：
ORI+CID+DID+DATA_FRAME
其中，
ORI为8bit（1byte）的数据流，为方向报文，用于判断传入的数据的转发方向，是记录到数据库中还是转发到客户端;
CID为8bit（1byte）的数据流，用于解析转发数据流的目标设备地址（即device表中的记录值）;
DID为8bit（1byte）的数据流，为数据类型报文，用于判断数据类型以便进行解析和记录;
DATA_FRAME为数据段，其中包含参数类信息包为定长包，包长度为全局定义的大小（包长定义为BUFFER_SIZE），因此DATA_FRAME的长度就是（BUFFER_SIZE-1-1）的结果。

ORI报文定义格式：


