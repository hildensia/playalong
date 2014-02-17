CXX = g++
LD = g++

CXXFLAGS=-g -std=c++0x
LDFLAGS=-lavcodec -lavformat -lavutil -lstdc++ -lm -lao -lncurses -lfftw3
OBJS = main.o player.o sound_stream.o curses_interface.o phase_vocoder.o util.o stft.o

default: playalong

playalong: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

install: playalong
	install playalong /usr/local/bin/playalong

clean:
	rm *.o
