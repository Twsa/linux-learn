# # Makefile for nps ingate exec, by denghuibin #
# *** TODO: (1) change 'test' to whatever you want for output file-name.
EXECUTABLE = run

CC = gcc
CCFLAGS  = -g 
COMPILE = $(CC) $(CCFLAGS)

# *** TODO: (2) add paths of include files.
INC  := -I./include 

# *** TODO: (3) add paths of source files.
SRCS := $(wildcard *.c) $(wildcard ./src/*.c)
OBJS := $(patsubst %.c,%.o, $(SRCS))
#DEPS := $(patsubst %.o,%.d, $(OBJS))

# *** TODO: (4) add library to project.
LIBS := 

#
# Output file
#
all: $(EXECUTABLE)
$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(LIBS)

#
# specify the dependency
#
%.d : %.c
	$(CC) $(INC) -MM $< > $@
	$(CC) $(INC) -MM $< -MQ $@  >> $@
#
# specify that all .o files depend on .c files, and indicate how to realize it
#

%.o : %.c
	$(COMPILE) -c $(INC) -o $@ $<

.PHONY: clean
clean:
	-rm $(EXECUTABLE)
	-rm $(OBJS)
	rm -f *.bak *.o
#	-rm $(DEPS)
#depend : $(DEPS)
#	@echo "Dependencies are now up-to-date."

#-include $(DEPS)
