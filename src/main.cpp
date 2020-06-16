//#include <map>
//#include <queue>
//#include <mutex>
//#include <iostream>
//#include <fstream>
//#include <sstream>

#include "../evp/gui.hpp"

#include "main_gui.hpp"

//-------------------------------------------------------- Main
int main()
{
  // ------------------------------------ WINDOW
  evp::GUI::MasterWindow* masterWindow = new evp::GUI::MasterWindow(1000,600,"window title",false);

  setUpBaseWindow(masterWindow->area());

  // ------------------------------------ MAIN
  while (masterWindow->isAlive()) {
    masterWindow->update();
    masterWindow->draw();
  }
}
