#include "sound_stream.h"
#include "player.h"
#include <string>
extern "C" {
#include "libavcodec/avcodec.h"
}
int main(int argc, char **argv) {
  av_register_all();
  SoundStream song(std::string("test.wav"));  

  Player player(song);
  player.set_marker(Marker_A, 1234);
  player.set_marker(Marker_B, 2345);

  player.play();
}
