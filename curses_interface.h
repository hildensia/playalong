#ifndef __CURSES_INTERFACE_H__
#define __CURSES_INTERFACE_H__

class Player;

class CursesInterface {
  private:
    Player& m_player;
    bool m_running;
    
  public:
    CursesInterface(Player& player);
    ~CursesInterface();
    void run();  
};

#endif
