# makefile example
RC_CC=~/Desktop/my_test/cross-compilers/host/bin/arm-rockchip-linux-gnueabihf-g++
CC=gcc
CFLAGS =
LFLAGS = 
DEBUG = -g

SRCS = timer_test_select.c

OBJS = $(patsubst %.c,%.o,$(SRCS))

release=pc.run
debug=pc.debug
pcrun:$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(release) $(LFLAGS)
	./$(release)
%.o:%.c
	$(CC) -c $(SRCS)
pcdb:$(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o  $(debug) $(LFLAGS) $(DEBUG)
	gdb ./$(debug)
%.o:%.c
	$(CC) -c $(SRCS)
rcrun:$(OBJS)
	$(RC_CC) $(CFLAGS) $(OBJS) -o $(release) $(LFLAGS)
	adb push $(release) /userdata
%.o:%.c
	$(RC_CC) -c $(SRCS)
.PHONY: clean
clean:
	rm -rf $(release) $(debug) $(OBJS)
