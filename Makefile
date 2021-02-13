CC = gcc
CFLAGS = 
OBJS = context-switch.o util.o

all: context-switch

context-switch: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o context-switch

context-switch.o: context_switch.c
	$(CC) $(CFLAGS) -c context-switch.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean:
	rm -f *.o context-switch
