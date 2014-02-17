CXX = g++
LD = g++

CXXFLAGS=-g -std=c++0x
LDFLAGS=-lavcodec -lavformat -lavutil -lstdc++ -lm -lao -lncurses -lfftw3 -gtest
OBJS = fftw.o ../src/stft.o ../src/phase_vocoder.o

default: pa_tests

pa_tests: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o
