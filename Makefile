OBJS = reciter.o sam.o render.o debug.o processframes.o createtransitions.o rendertabs.o wordlist.o

CC = g++

# libsdl present
CFLAGS =  -Wall -DUSESDL `sdl-config --cflags` -g
LFLAGS = `sdl-config --libs`

# no libsdl present
#CFLAGS =  -Wall -O2
#LFLAGS =

all: sam parser

sam: $(OBJS) main.o
	$(CC) -o sam $(OBJS) main.o $(LFLAGS)

parser: $(OBJS) parser.o
	$(CC) -o parser $(OBJS) parser.o

%.o: src/sam/%.cc
	$(CC) $(CFLAGS) -c $<

%.o: src/tools/%.cc
	$(CC) $(CFLAGS) -c $<

%.o: src/%.cc
	$(CC) $(CFLAGS) -c $<

package:
	tar -cvzf sam.tar.gz README.md Makefile sing src/

clean:
	rm *.o
