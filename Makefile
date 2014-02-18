CXX = g++
LD = g++

CXXFLAGS=-g -std=c++0x
LDFLAGS=-lavcodec -lavformat -lavutil -lstdc++ -lm -lao -lncurses

OBJS = main.o player.o sound_stream.o curses_interface.o util.o

ifndef DESTDIR
DESTDIR=/usr/local
endif

default: playalong

playalong: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

install: playalong
	install playalong $(DESTDIR)/bin/playalong

clean:
	rm *.o
