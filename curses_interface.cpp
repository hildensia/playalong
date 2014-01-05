#include "curses_interface.h"
#include "player.h"

#include <ncurses.h>

CursesInterface::CursesInterface(Player& player) : m_player(player), m_running(true) {
  initscr();

  cbreak();
  keypad(stdscr, true);
  noecho();
}
void CursesInterface::run() {
  while(m_running) {
    int ch = getch();
    switch(ch) {
      case KEY_LEFT:
        m_player.seek(m_player.get_pos() - 500);  
        break;
      case KEY_RIGHT:
        m_player.seek(m_player.get_pos() + 500);  
        break;
      case 'd':
        m_player.set_marker(Marker_A, m_player.get_pos());
        break;
      case 'e':
        m_player.set_marker(Marker_A, 0);
        break;
      case 'f':
        m_player.set_marker(Marker_A, m_player.get_marker(Marker_A) + 10);
        break;
      case 's':
        m_player.set_marker(Marker_A, m_player.get_marker(Marker_A) - 10);
        break;
      case 'j':
        m_player.set_marker(Marker_B, m_player.get_pos());
        break;
      case 'u':
        m_player.set_marker(Marker_B, 0);
        break;
      case 'k':
        m_player.set_marker(Marker_B, m_player.get_marker(Marker_B) + 10);
        break;
      case 'h':
        m_player.set_marker(Marker_B, m_player.get_marker(Marker_B) - 10);
        break;
      case ' ':
        m_player.pause();
        break;
    }
    refresh();
  }
}

CursesInterface::~CursesInterface() {
  endwin();  
}
