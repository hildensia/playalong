#ifndef __PHASE_VOCODER_H__
#define __PHASE_VOCODER_H__

#include "types.h"
#include "filter.h"
#include <vector>
#include <complex>
#include <fftw3.h>

double hanning(const int n, const int M);
double hamming(const int n, const int M);
double reect(const int n, const int M);

stft_t phase_adjustment(stft_t& data, int bin_num, double stretching);

class PhaseVocoder : public Filter {
  private:

    stft_t m_stft_data_left, m_stft_data_right;
    std::vector<double> m_data_left, m_data_right;

    double m_pos;
    double m_speed_factor;
    
    uint16_t m_window_size;

    fftw_complex* m_slice;
    double* m_frame;

    fftw_plan m_istft;

  public:
    PhaseVocoder(SoundStream& input);
    virtual uint16_t *get_next_frame(uint& frame_size);
    virtual pos_t get_pos();
    virtual void set_pos(pos_t ms);
    virtual pos_t get_duration(); 
    virtual ~PhaseVocoder() { };
};

#endif
