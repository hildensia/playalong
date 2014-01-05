#ifndef __CURSES_INTERFACE_H__
#define __CURSES_INTERFACE_H__

class Player;

class CursesInterface {
  private:
    Player& m_player;
    bool m_running;

    void key_management();
    void init_window();
    void render_window();

    int m_old_x, m_old_y;

  public:
    CursesInterface(Player& player);
    ~CursesInterface();
    void run();  
};

#endif
