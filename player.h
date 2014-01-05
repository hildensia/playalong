#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <ao/ao.h>

class SoundStream;

enum marker_t { Marker_A, Marker_B } ;

class Player {
  private:
    int m_a_marker, m_b_marker;
    bool m_stop;
    SoundStream& m_song;
    
    int m_driver;
    ao_sample_format m_sformat;

  public:
    Player(SoundStream& song);

    void play();
    void stop();
    
    void seek(int ms_pos);
    int get_pos();

    int get_marker(const marker_t marker) const;
    void set_marker(const marker_t marker, const int ms_pos);
};

#endif
