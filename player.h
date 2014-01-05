#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <ao/ao.h>
#include <thread>
#include <atomic>

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
    void detached_play();

  public:
    Player(SoundStream& song);

    void play();
    void pause();
    void stop();
    
    void seek(int ms_pos);
    int get_pos();

    int get_marker(const marker_t marker) const;
    void set_marker(const marker_t marker, const int ms_pos);
};

#endif
