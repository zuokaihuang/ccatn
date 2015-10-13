EXECS = ccatn client

CC = gcc
INCLUDES = -I.
KDFKAGS = 
LDLIBS = 

DSOURCES = ccatn.c \
		 epoll_ctl.c \
		 packet_ctl.c \
		 data_st.c
		 
CSOURCES = client.c 
		
DOBJECTS = $(DSOURCES:.c=.o)
COBJECTS = $(CSOURCES:.c=.o)

CFLAGS += -Wall -D__LINUX__  -I.

all: $(EXECS)

ccatn: $(DOBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(DOBJECTS)
client: $(COBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(COBJECTS)
		
clean:
	rm -f $(EXECS) *.elf *.gdb *.o
	
