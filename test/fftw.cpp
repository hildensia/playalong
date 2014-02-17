#include <vector>
#include <iostream>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <fstream>
#include <functional>
#include <cstring>

#include "../src/stft.h"
#include "../src/phase_vocoder.h"
#include "../src/types.h"
#include "../src/util.h"

using std::cout;
using std::endl;


istft_t gen_sin(uint length) {
  istft_t d;
  for (int i=0; i<length; i++) {
    d.push_back(sin((double) (i - 13) / 31.) + sin((double) (i -17) / 47.));
  }  
  return d;
}


int main() {
  int length = 65500;
  int window = 1024;
  int hop = window / 4;
  istft_t d = gen_sin(length);
  stft_t fft = stft(d, hanning, window, hop);
  stft_t adj = phase_adjustment(fft, window, 1.2);
  istft_t ifft = scale(istft(adj, hanning, window, hop));

  std::ofstream out;
  out.open("before.data");
  for (uint i = 0; i < length; ++i) {
    out << d[i] << ", ";  
  }
  out.close();

  out.open("after.data");
  for (uint i = 0; i < length; ++i) {
    out << ifft[i] << ", ";  
  }
  out.close();
}
