#include "player.h"
#include "sound_stream.h"
#include <ao/ao.h>
#include <iostream>

extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

Player::Player(SoundStream& song) : m_stop(true), m_song(song) {
  //initialize AO lib
  ao_initialize();
  m_driver = ao_default_driver_id();

  AVSampleFormat sfmt = song.get_sample_fmt();
  if(sfmt==AV_SAMPLE_FMT_U8 || sfmt==AV_SAMPLE_FMT_U8P){
    m_sformat.bits=16;
  } else if(sfmt==AV_SAMPLE_FMT_S16 || sfmt==AV_SAMPLE_FMT_S16P){
    m_sformat.bits=16;
  } else if(sfmt==AV_SAMPLE_FMT_S32 || sfmt==AV_SAMPLE_FMT_S32P){
    m_sformat.bits=16;
  } else if(sfmt==AV_SAMPLE_FMT_FLT || sfmt==AV_SAMPLE_FMT_FLTP) {
    m_sformat.bits=16;
  } else if(sfmt==AV_SAMPLE_FMT_DBL || sfmt==AV_SAMPLE_FMT_DBLP) {
    m_sformat.bits=16;
  } else {
    printf("Unsupported format");
  }

  m_sformat.channels = song.get_channels();
  m_sformat.rate = song.get_sample_rate();
  m_sformat.byte_format = AO_FMT_NATIVE;
  m_sformat.matrix = 0;
}

void Player::play() {
  ao_device *adevice = ao_open_live(m_driver, &m_sformat, NULL);
  m_stop = false;
  while(!m_stop) {
    if (m_song.get_pos() > m_b_marker)
      m_song.set_pos(m_a_marker);
    uint frame_size;
    uint16_t *frame = m_song.get_next_frame(frame_size);
    ao_play(adevice, (char*) frame, frame_size);
  } 
  ao_shutdown();
}

void Player::stop() {
  m_stop = true;
}
  
void Player::seek(int ms_pos) {
  m_song.set_pos(ms_pos);  
}
int Player::get_pos() {
  return m_song.get_pos();  
}

int Player::get_marker(const marker_t marker) const {
  if (marker == Marker_A) return m_a_marker;  
  else return m_b_marker;  
}
void Player::set_marker(const marker_t marker, const int ms_pos) {
  if (marker == Marker_A) m_a_marker = ms_pos;  
  else m_b_marker = ms_pos;
}
