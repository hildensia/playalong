CXX = g++
LD = g++

CXXFLAGS=-g
LDFLAGS=-lavcodec -lavformat -lavutil -lstdc++ -lm -lao

OBJS = main.o player.o sound_stream.o

default: playalong

playalong: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o
