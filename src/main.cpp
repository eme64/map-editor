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
// # Data Layers: list of layers - for now string only, text displayed.
// - Objects: key-value pairs. Icons + names?
// - Fill Area, other grid manipulation tools
//
// Details to Objects:
// - are only a point (id,x,y,name) + dict
// - if dict.icon set will try to load a pic and display
// - list of objects, list to edit selected object (dict)
//
// next steps: Objects: select/drag/move. Save/Load. Dict. Icon.

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
