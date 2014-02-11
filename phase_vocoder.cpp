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

double hamming(const int n, const int M) {
  return .54 + .46 * cos(2 * M_PI * n)/M;
}

double reect(const int n, const int M) {
  return 1;  
}
    
void save(stft_t data, uint window_size, const char* file) {
  std::ofstream out(file);

  for (fftw_complex *d : data) {
    for (uint i = 0; i<window_size; ++i) {
      out << d[i][0] << " ";
    }
    out << std::endl;
  }  

  out.close();
}

void save(std::vector<double> data, const char* file) {
  std::ofstream out(file);
  for (uint i = 0; i<data.size(); i++)
    out << data[i] << " ";
  out.close();
}

stft_t time_strech(stft_t& data, double factor) {
  stft_t streched(data.size() * factor);
  for (uint i = 0; i < streched.size(); ++i) {
    streched[i] = data[(int) (i/factor)];  
  }
  return streched;
}

inline double phase_adjustment(double phi_i, double phi_prev, double delta_t, double bin_freq) {
  double delta_w = ((phi_i - phi_prev)/delta_t) - bin_freq;
  double delta_w_wrapped = fmod((delta_w + M_PI), (2 * M_PI)) - M_PI;
  double w_true = bin_freq + delta_w_wrapped;
  return phi_prev + delta_t * w_true;
}

void phase_adjustment(stft_t& data, double sample_freq, int hop_size, int bin_num) {
  double delta_t = hop_size / sample_freq;  
  for(fftw_complex* cpx : data) {
    for (uint i = 1; i < bin_num; ++i) {
      cpx[i][1] = phase_adjustment(cpx[i][1], cpx[i-1][1], delta_t, (i/(double)bin_num) * sample_freq);
    }  
  }
}

PhaseVocoder::PhaseVocoder(SoundStream& input) : Filter(input), m_pos(0), m_speed_factor(1.), m_window_size(2048) {
  pos_t old_pos = 0;
  pos_t pos = 1;
  uint size;
  std::cout << "Reading file..." << std::flush;
  std::vector<uint16_t> data;

  int hop_size = m_window_size *.5;

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
  
  m_stft_data_left = stft(m_data_left, hamming, m_window_size, hop_size);
  m_stft_data_right = stft(m_data_right, hamming, m_window_size, hop_size);

  phase_adjustment(m_stft_data_left, input.get_sample_rate(), hop_size, m_window_size/2+1);
  phase_adjustment(m_stft_data_right, input.get_sample_rate(), hop_size, m_window_size/2+1);

  m_data_left = scale(istft(m_stft_data_left, hamming, m_window_size, hop_size));
  m_data_right = scale(istft(m_stft_data_right, hamming, m_window_size, hop_size));
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
