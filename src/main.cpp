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
  EP::GUI::MasterWindow* masterWindow = new EP::GUI::MasterWindow(1000,600,"window title",false);

  setUpBaseWindow(masterWindow->area());

  // ------------------------------------ MAIN
  while (masterWindow->isAlive()) {
    masterWindow->update();
    masterWindow->draw();
  }
}
