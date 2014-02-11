#ifndef _STFT_H_
#define _STFT_H_

#include "types.h"

stft_t stft(std::vector<double> data, window_fct_t window_function, uint window_size, uint step_size);
istft_t istft(stft_t data, window_fct_t window_function, uint window_size, uint step_size);

#endif
