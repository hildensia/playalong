#include "phase_vocoder.h"
#include "util.h"
#include "types.h"
#include "stft.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>


using std::cout;
using std::endl;

double hanning(const int n, const int M) {
  return .5 * (1 - cos((2 * M_PI * n)/(M-1)));
}

double hamming(const int n, const int M) {
  return .54 + .46 * cos(2 * M_PI * n)/M;
}

double reect(const int n, const int M) {
  return 1;  
}

stft_t phase_adjustment(stft_t& data, int bin_num, double stretching) {
  stft_t stretched(data.size() * stretching);
  for(uint i = 0; i<stretched.size(); ++i) {
    stretched[i] = (fftw_complex*) fftw_malloc(bin_num * sizeof(fftw_complex));
  }

  for (uint bin = 0; bin < bin_num/2 + 1; ++bin) {
    stretched[0][bin][0] = data[0][bin][0];
    stretched[0][bin][1] = data[0][bin][1];
  }


  for (uint frame = 0; frame < stretched.size()-1; frame++) {
    double data_pos = frame / stretching;
    uint data_frame = (uint) data_pos;
    double frac = data_pos - data_frame;
    cout << "frame = " << frame << endl;
    cout << "data_pos = " << data_pos << endl;
    cout << "data_frame = " << data_frame << endl;
    cout << "frac = " << frac << endl;

    for (uint bin = 0; bin < bin_num/2 + 1; ++bin) {

      std::complex<double> *curr = reinterpret_cast<std::complex<double>*>(data[data_frame][bin]);
      std::complex<double> *next = reinterpret_cast<std::complex<double>*>(data[data_frame+1][bin]);

      // amplitude interpolation
      double amp = (1-frac) * std::abs(*curr) + frac * std::abs(*next);

      // Calculate phase advance
      // Phase accumulator

      double delta_phi = std::arg(*next) - std::arg(*curr);
      while (delta_phi > M_PI) delta_phi -= 2*M_PI;
      while (delta_phi < -M_PI) delta_phi += 2*M_PI;

      double phase = std::arg(*reinterpret_cast<std::complex<double>*>(stretched[frame][bin])) + delta_phi;

      std::complex<double> s = std::polar(amp, phase);
      stretched[frame + 1][bin][0] = std::real(s);
      stretched[frame + 1][bin][1] = std::imag(s);
    }
  }
  return stretched;
}



PhaseVocoder::PhaseVocoder(SoundStream& input) : Filter(input), m_pos(0), m_speed_factor(1.), m_window_size(1024) {
  pos_t old_pos = 0;
  pos_t pos = 1;
  uint size;
  std::cout << "Reading file..." << std::flush;
  std::vector<uint16_t> data;


  while(pos > old_pos) {
    old_pos = pos;
    uint16_t *d = input.get_next_frame(size);
    pos = input.get_pos();
    for (uint i=0; i<size; i+=2 ) { // += in_channels) {
      m_data_left.push_back(d[i]/(double) std::numeric_limits<uint16_t>::max());
      m_data_right.push_back(d[i+1]/(double) std::numeric_limits<uint16_t>::max());
    }
    free(d);
  }
  std::cout << "done." << std::endl;
  
  int hop_size = m_window_size / 4;
  double scaler = 1.2;

  m_stft_data_left = stft(m_data_left, hanning, m_window_size, hop_size);
  m_stft_data_right = stft(m_data_right, hanning, m_window_size, hop_size);

  stft_t scaled_data_left = phase_adjustment(m_stft_data_left, m_window_size, scaler);
  stft_t scaled_data_right = phase_adjustment(m_stft_data_right, m_window_size, scaler);

  m_data_left = scale(istft(scaled_data_left, hanning, m_window_size, hop_size));
  m_data_right = scale(istft(scaled_data_right, hanning, m_window_size, hop_size));
}

uint16_t *PhaseVocoder::get_next_frame(uint& frame_size) {
  uint ws = 2048; 
  uint16_t *result = new uint16_t[2*ws];
  for (uint i=0; i<ws/2; i++) {
    result[2*i] = (uint16_t) (m_data_left[m_pos + i] * std::numeric_limits<uint16_t>::max());
    result[2*i+1] = (uint16_t) (m_data_right[m_pos + i] * std::numeric_limits<uint16_t>::max());
  }
  m_pos += ws/2;
  frame_size = 2*ws;
  return result;
}

pos_t PhaseVocoder::get_pos() {
  return m_pos/(m_data_left.size() * 2) * input.get_duration();  
}

void PhaseVocoder::set_pos(pos_t ms) {
  m_pos = (ms / (double) input.get_duration()) * (m_data_left.size() * 2);
}
