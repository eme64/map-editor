#include <map>
#include <queue>
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../evp/gui.hpp"

//-------------------------------------------------------- Main
int main()
{
  // ------------------------------------ WINDOW
  EP::GUI::MasterWindow* masterWindow = new EP::GUI::MasterWindow(1000,600,"window title",false);
  {
    EP::GUI::Window* window2 = new EP::GUI::Window("window2",masterWindow->area(),300,150,600,400,"Designer");
    {
      float x,y,dx,dy;
      window2->childSize(dx,dy);
      window2->childOffset(x,y);
      
      EP::GUI::TextInput* inp = new EP::GUI::TextInput("input",window2,x+10,y+10,dx-20,30,"asdf");
      inp->onTextIs([](const std::string& s){
        std::cout << "Text update:"<< s <<"\n";
      });
    }


    EP::GUI::Window* window1 = new EP::GUI::Window("window1",masterWindow->area(),10,10,200,300,"MyWindow1");
    EP::GUI::Window* w = window1;
    for (size_t i = 3; i < 7; i++) {
      float x,y,dx,dy;
      w->childSize(dx,dy);
      w->childOffset(x,y);
      EP::GUI::Area* content1 = new EP::GUI::Area("content"+std::to_string(i),w,x,y,dx,dy);
      content1->colorIs(EP::Color(0.2,0.2,0.3));
      content1->fillParentIs(true);
      EP::GUI::Window* ww = new EP::GUI::Window("w"+std::to_string(i),content1,10,10,100,100,"w"+std::to_string(i));
      w = ww;
    }

  }
 


  // ------------------------------------ MAIN
  while (masterWindow->isAlive()) {
    masterWindow->update();
    masterWindow->draw();
  }
}
