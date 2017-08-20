OBJS = reciter.o sam.o render.o main.o debug.o processframes.o createtransitions.o rendertabs.o

CC = g++

# libsdl present
CFLAGS =  -Wall -DUSESDL `sdl-config --cflags` -g
LFLAGS = `sdl-config --libs`

# no libsdl present
#CFLAGS =  -Wall -O2
#LFLAGS =

sam: $(OBJS)
	$(CC) -o sam $(OBJS) $(LFLAGS)

%.o: src/%.cc
	$(CC) $(CFLAGS) -c $<

package:
	tar -cvzf sam.tar.gz README.md Makefile sing src/

clean:
	rm *.o
