#include "stft.h"
#include <fftw3.h>
#include <cstring>
#include <functional>

stft_t stft(std::vector<double> data, window_fct_t window_function, uint window_size, uint hop_size) {
  double *_data = (double*) fftw_malloc(sizeof(double) * window_size);
  fftw_complex *_stft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
  stft_t result;
  
  fftw_plan forward = fftw_plan_dft_r2c_1d(window_size, _data, _stft, FFTW_ESTIMATE);

  for (uint pos = 0; pos < data.size(); pos += hop_size) {
    for (uint i = 0; i < window_size; ++i) {
      if (pos + i < data.size())
        _data[i] = window_function(i, window_size) * data[pos + i];  
      else
        _data[i] = 0;
    }
    fftw_execute(forward);

    fftw_complex *stft_slice = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    memcpy(stft_slice, _stft, sizeof(fftw_complex) * window_size);
    result.push_back(stft_slice);
  }

  fftw_destroy_plan(forward);
  fftw_free(_data);

  return result;
}

istft_t istft(stft_t data, window_fct_t window_function, uint window_size, uint hop_size) {
  double *frame = (double*) fftw_malloc(sizeof(double) * window_size);
  fftw_complex *slice = (fftw_complex*) malloc(sizeof(fftw_complex) * window_size);
  fftw_plan istft = fftw_plan_dft_c2r_1d(window_size, slice, frame, FFTW_ESTIMATE);

  istft_t result(data.size() * hop_size + (window_size - hop_size));
  for (uint i=0; i<result.size(); ++i) result[i] = 0;

  for (uint i = 0; i < data.size(); ++i) {
    memcpy(slice, data[i], sizeof(fftw_complex) * window_size);
    fftw_execute(istft);

    for (uint pos = 0; pos < window_size; ++pos) {
      uint r_pos = pos + i * hop_size;
      result[r_pos] += frame[pos] * window_function(pos, window_size);
    } 
  }
  return result;
}
