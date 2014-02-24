#include "curses_interface.h"
#include "player.h"
#include "util.h"

#include <curses.h>

CursesInterface::CursesInterface(Player& player) : m_player(player), m_running(true), m_old_x(0), m_old_y(0) {
  initscr();
  timeout(15);
  //SCREEN *s = newterm(NULL, stdin, stdout);

  cbreak();
  noecho();
  keypad(stdscr, true);
}
void CursesInterface::run() {
  init_window();
  while(m_running) {
    key_management();
    render_window();
  }
}

void CursesInterface::init_window() {
}
void CursesInterface::render_window() {
  pos_t marker_a_pos = m_player.get_marker(Marker_A);
  pos_t marker_b_pos = m_player.get_marker(Marker_B);
  pos_t pos = m_player.get_pos();
  pos_t end = m_player.get_end();

  int x, y;
  getmaxyx(stdscr, y, x);
  if (m_old_x != x || m_old_y != y) {
    clear();
    m_old_x = x;
    m_old_y = y;
  }

  metadata_t metadata = m_player.get_metadata();
  int titlelength = 3 + metadata["artist"].size() + metadata["title"].size();
  
  mvprintw(3, (x-titlelength)/2, "%s - %s", metadata["artist"].c_str(), metadata["title"].c_str()); 
  mvprintw(8, (x-ms_to_readable(pos).size()-4)/2-1, "Pos: %s", ms_to_readable(pos).c_str());
  mvprintw(5, 1, "[");
  mvprintw(5, x-2, "]");
  for (uint p=2; p<x-2; ++p)
    mvprintw(5, p, "=");
  mvprintw(5, 2 + (int) (x-4)*(pos/(double) end), "#");
  if(marker_a_pos)
    mvprintw(6, 2 + (int) (x-4)*(marker_a_pos/(double) end), "^");
  if(marker_b_pos)
    mvprintw(6, 2 + (int) (x-4)*(marker_b_pos/(double) end), "^");
  mvprintw(9, (x-ms_to_readable(end).size()-3)/2, "of: %s", ms_to_readable(end).c_str());
  mvprintw(8, 1, "Marker A: %s", ms_to_readable(marker_a_pos).c_str());
  mvprintw(8, x-23, "Marker B: %s", ms_to_readable(marker_b_pos).c_str());

  refresh();
}

void CursesInterface::key_management() {
  int ch = getch();
  switch(ch) {
    case KEY_LEFT:
      m_player.seek(m_player.get_pos() - 500);  
      break;
    case KEY_RIGHT:
      m_player.seek(m_player.get_pos() + 500);  
      break;
    case KEY_SLEFT:
      m_player.seek(m_player.get_pos() - 5000);  
      break;
    case KEY_SRIGHT:
      m_player.seek(m_player.get_pos() + 5000);  
      break;
    case KEY_DOWN:
      m_player.seek(m_player.get_pos() - 30000);  
      break;
    case KEY_UP:
      m_player.seek(m_player.get_pos() + 30000);  
      break;
    case KEY_PPAGE:
      m_player.seek(m_player.get_pos() - 300000);  
      break;
    case KEY_NPAGE:
      m_player.seek(m_player.get_pos() + 300000);  
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
    case 'q':
      m_player.stop();
      m_running = false;
      break;
  }
}

CursesInterface::~CursesInterface() {
  endwin();  
}
