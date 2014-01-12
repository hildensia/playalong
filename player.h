#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <ao/ao.h>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "types.h"

class SoundStream;

enum marker_t { Marker_A, Marker_B } ;

class Player {
  private:
    std::atomic<int> m_a_marker, m_b_marker;
    std::atomic<bool> m_stop;
    std::atomic<bool> m_pause;

    SoundStream& m_song;
    
    int m_driver;
    ao_sample_format m_sformat;

    std::thread m_player_thread;

    std::mutex m_pause_mtx;
    std::condition_variable m_pause_cv;

    void detached_play();

  public:
    Player(SoundStream& song);

    void play();
    void pause();
    void stop();
    
    void seek(pos_t ms_pos);
    pos_t get_pos();

    pos_t get_end();

    pos_t get_marker(const marker_t marker) const;
    void set_marker(const marker_t marker, const pos_t ms_pos);

    metadata_t get_metadata();
};

#endif
