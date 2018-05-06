cc=/home/xuegao/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin/mipsel-openwrt-linux-musl-gcc
#cc=gcc

target=server_mt7620
#target=server

option= -lpthread

obj=server.o linux_mq.o message_handling.o toolbox.o 






$(target):$(obj)
	$(cc) $(obj) -o $(target)  $(option)
server.o:server.c
	$(cc) -c server.c $(option)
linux_mq.o:linux_mq.c
	$(cc) -c linux_mq.c $(option)
message_handling.o:message_handling.c
	$(cc) -c message_handling.c $(option)
toolbox.o:toolbox.c
	$(cc) -c toolbox.c $(option)
.PHONY:clean
	$(RM) *.o $(target) 
