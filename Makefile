# udpCode makefile
# written by Hugh Smith - Feb 2021

CC = gcc
CFLAGS = -g -Wall


SRC = networks.c  gethostbyname.c pdu.c window.c srej.c
OBJS = networks.o gethostbyname.o pdu.o window.o srej.o

#uncomment next two lines if your using sendtoErr() library
LIBS += libcpe464.2.21.a -lstdc++ -ldl
CFLAGS += -D__LIBCPE464_

all:  udpClient udpServer winTester

udpClient: rcopy.c $(OBJS)
	$(CC) $(CFLAGS) -o rcopy rcopy.c $(OBJS) $(LIBS)

udpServer: server.c $(OBJS)
	$(CC) $(CFLAGS) -o server server.c  $(OBJS) $(LIBS)

winTester: winTest.c $(OBJS)
	$(CC) $(CFLAGS) -o winTest winTest.c  $(OBJS) $(LIBS)

%.o: %.c *.h 
	gcc -c $(CFLAGS) $< -o $@ 

cleano:
	rm -f *.o

clean:
	rm -f rcopy server *.o

