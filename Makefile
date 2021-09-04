TARGET	= client
#SRCS	= client.c connection.c checkInfo.c cardSelect.c cardChange.c bitCard.c handGenerator.c estimateHand.c mt19937ar.c
SRCS	= client.c connection.c checkInfo.c cardSelect.c cardChange.c bitCard.c handGenerator.c mt19937ar.c makeData.c
#OBJS	= client.o connection.o checkInfo.o cardSelect.o cardChange.o bitCard.o handGenerator.o estimateHand.o mt19937ar.o
OBJS	= client.o connection.o checkInfo.o cardSelect.o cardChange.o bitCard.o handGenerator.o mt19937ar.o makeData.o

CC = gcc
LIBS = -lm  
CFLAGS = -g -O2

all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)


Makefile: Makefile.in
	./configure

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

client.o: mydef.h
checkInfo.o: mydef.h
cardSelect.o: mydef.h
bitCard.o: mydef.h
handGenerator.o: mydef.h
estimateHand.o: mydef.h
makeData.o: makeData.h
