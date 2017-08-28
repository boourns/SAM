OBJS = reciter.o sam.o render.o debug.o processframes.o createtransitions.o rendertabs.o
SAM_OBJS = $(OBJS) main.o
PARSER_OBJS = $(OBJS) parser.o
VOCALIST_OBJS = $(OBJS) vocalist.o wordlist.o vocalist_main.o

CC = g++

# libsdl present
CFLAGS =  -Wall -DUSESDL `sdl-config --cflags` -g
LFLAGS = `sdl-config --libs`

# no libsdl present
#CFLAGS =  -Wall -O2
#LFLAGS =

all: sam parser vocalist

src/wordlist.cc: parser src/tools/words.rb
	ruby ./src/tools/words.rb

sam: $(SAM_OBJS)
	$(CC) -o sam $(SAM_OBJS) $(LFLAGS)

parser: $(PARSER_OBJS)
	$(CC) -o parser $(PARSER_OBJS)

vocalist: $(VOCALIST_OBJS)
	$(CC) -o vocalist $(VOCALIST_OBJS) $(LFLAGS) -lrtmidi

%.o: src/sam/%.cc
	$(CC) $(CFLAGS) -c $<

%.o: src/tools/%.cc
	$(CC) $(CFLAGS) -c $<

%.o: src/%.cc
	$(CC) $(CFLAGS) -c $<

package:
	tar -cvzf sam.tar.gz README.md Makefile sing src/

clean:
	rm *.o ./src/wordlist.cc
