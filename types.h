#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>
#include <map>
#include <string>
#include <functional>
#include <vector>
#include <fftw3.h>

typedef int64_t pos_t;
typedef std::map<std::string, std::string> metadata_t;
typedef std::function<double(const int, const int)> window_fct_t;
typedef std::vector<fftw_complex*> stft_t;
typedef std::vector<double> istft_t;

#endif
