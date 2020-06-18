//#include <map>
//#include <queue>
//#include <mutex>
//#include <iostream>
//#include <fstream>
//#include <sstream>

#include "../evp/gui.hpp"

#include "main_gui.hpp"

// Ideas:
//
// Done: #
// Open: -
//
// # New: set size and number points
// - Data Layers: list of layers (int, float, string?) - visualization and coloring?
// - Objects: key-value pairs. Icons + names?

//-------------------------------------------------------- Main
int main()
{
  // ------------------------------------ WINDOW
  evp::GUI::MasterWindow* masterWindow = new evp::GUI::MasterWindow(1000,600,"Voronoi Map Editor",false);

  setUpBaseWindow(masterWindow->area());

  // ------------------------------------ MAIN
  while (masterWindow->isAlive()) {
    masterWindow->update();
    masterWindow->draw();
  }
}
