#include "util.h"
#include <iomanip>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

extern "C" {
#include "libavutil/dict.h"
}

void ms_to_readable(const int ms, int& h, int& m, int& s, int& mms) {
  mms = ms % 1000;
  int rest = (ms - mms) / 1000;
  s = rest % 60;
  rest = (rest - s) / 60;
  m = rest % 60;
  h = (rest - m) / 60;
}

std::string ms_to_readable(const int ms) {
  int h, min, s, _ms;
  ms_to_readable(ms, h, min, s, _ms);
  std::stringstream tmp;
  tmp << std::setfill('0');
  tmp << h << ":" << std::setw(2) << min << ":" << std::setw(2) << s << "." << std::setw(3) << _ms;
  return tmp.str();
}

metadata_t av_dict_to_metadata(AVDictionary* dict) {
  metadata_t metadata;
  AVDictionaryEntry *tag = NULL;
  while ((tag = av_dict_get(dict, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    metadata[tag->key] = tag->value;  
  }
  return metadata;
}
void print_complex(fftw_complex* cpx, size_t size) {
  for (uint i = 0; i < size; ++i) {
    cout << cpx[i][0] << " + " << cpx[i][1] << endl;
  }  
}
