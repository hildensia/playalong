#include "player.h"
#include "sound_stream.h"
#include "util.h"
#include <ao/ao.h>
#include <iostream>
#include <thread>

extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

Player::Player(SoundStream& song) : m_a_marker(0), m_b_marker(0), m_stop(true), m_pause(false), m_song(song) {
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

void Player::detached_play() {
  ao_device *adevice = ao_open_live(m_driver, &m_sformat, NULL);
  m_stop = false;
  while(!m_stop) {
    if (m_pause) {
      std::unique_lock<std::mutex> lck(m_pause_mtx);  
      m_pause_cv.wait(lck);
    }

    if (m_b_marker && m_song.get_pos() > m_b_marker)
      m_song.set_pos(m_a_marker);
    uint frame_size;
    uint16_t *frame = m_song.get_next_frame(frame_size);
    ao_play(adevice, (char*) frame, frame_size);
    free(frame);
  } 
  ao_shutdown();
}

void Player::play() {
  m_player_thread = std::thread(&Player::detached_play, this);  
}

void Player::stop() {
  m_stop = true;
  m_pause_cv.notify_all();
  m_player_thread.join();
}

void Player::pause() {
  m_pause = !m_pause;  
  if(!m_pause)
    m_pause_cv.notify_all();
}
  
void Player::seek(pos_t ms_pos) {
  ms_pos = limit<int>(ms_pos, 0, get_end());
  m_song.set_pos(ms_pos);  
}

pos_t Player::get_pos() {
  return m_song.get_pos();  
}

pos_t Player::get_end() {
  return m_song.get_duration();
}

pos_t Player::get_marker(const marker_t marker) const {
  if (marker == Marker_A) return m_a_marker;  
  else return m_b_marker;  
}

void Player::set_marker(const marker_t marker, const pos_t ms_pos) {
  pos_t pos = limit<pos_t>(ms_pos, 0, get_end());

  if (marker == Marker_A) {
    if (m_b_marker == 0 || pos <= m_b_marker)
      m_a_marker = pos;   
  }
  else {
    if (pos > m_a_marker || pos == 0)
      m_b_marker = pos;  
  }
}

metadata_t Player::get_metadata() {
  return m_song.get_metadata();  
}
