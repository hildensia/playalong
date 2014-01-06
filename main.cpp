#include "sound_stream.h"
#include "player.h"
#include "curses_interface.h"
#include <string>
#include <iostream>
extern "C" {
#include "libavcodec/avcodec.h"
}
int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: playalong <AUDIO_FILE>" << std::endl;
    exit(0);  
  }
  std::string file(argv[1]);
  
  av_register_all();
  av_log_set_level(AV_LOG_QUIET);

  SoundStream song(file);  

  Player player(song);
  player.play();

  CursesInterface interface(player);
  interface.run();

}
