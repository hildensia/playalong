#ifndef __UTIL_H__
#define __UTIL_H__

#include "types.h"

#include <string>
#include <map>

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
#endif
