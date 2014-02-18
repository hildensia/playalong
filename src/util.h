#ifndef __UTIL_H__
#define __UTIL_H__

#include "types.h"

#include <string>
#include <map>
#include <algorithm>

struct AVDictionary;

template<class T>
T limit(const T cur, const T min, const T max) {
  if (cur > max) return max;
  else if (cur < min) return min;
  else return cur;  
}

void ms_to_readable(const int ms, int& h, int& m, int& s, int& mms);
std::string ms_to_readable(const int ms);

metadata_t av_dict_to_metadata(AVDictionary* dict);

void print_complex(fftw_complex* cpx, size_t size);

template<class T>
std::vector<T> scale(std::vector<T> v) {
  auto m = std::max_element(v.begin(), v.end());
  std::vector<T> r(v.size());
  for (uint i=0; i<v.size(); ++i)
    r[i] = v[i]/(*m);
  return r;
}
template<class T>
std::vector<T> scale(std::vector<T> v, double max) {
  std::vector<T> r(v.size());
  for (uint i=0; i<v.size(); ++i)
    r[i] = v[i]/max;
  return r;
}
#endif
