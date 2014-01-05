#include "sound_stream.h"
#include "player.h"
#include "curses_interface.h"
#include <string>
extern "C" {
#include "libavcodec/avcodec.h"
}
int main(int argc, char **argv) {
  av_register_all();
  av_log_set_level(AV_LOG_QUIET);
  SoundStream song(std::string("test.mp3"));  

  Player player(song);
  player.play();

  CursesInterface interface(player);
  interface.run();

}
