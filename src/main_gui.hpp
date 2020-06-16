#include "../evp/gui.hpp"


class Editor {
public:
  Editor(EP::GUI::Area* const parent) {
    window = new EP::GUI::Window("editor",parent,200,200,300,300,"Editor");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);
 
    fileName = new EP::GUI::TextInput("fileNameInput",window,x,y,200,20,"file.txt");

    EP::GUI::Button* loadb = new EP::GUI::Button("buttonLoad",window,x+205,y,50,20,"Load");
    EP::GUI::Button* storeb = new EP::GUI::Button("buttonStore",window,x+265,y,50,20,"Store");
    loadb->onClickIs([this]() {
      load();
    });
    storeb->onClickIs([this]() {
      store();
    });
 

    EP::GUI::Area* content = new EP::GUI::Area("contentArea",NULL,0,0,100,400);
    EP::GUI::Area* scroll = new EP::GUI::ScrollArea("scroll",window,content,x,y+50,dx,dy);
    content->colorIs(EP::Color(0.2,0,0));
    scroll->fillParentIs(true,true);
  }
  void load() {std::cout << "**LOAD**\n";}
  void store() {std::cout << "**STORE**\n";}
private:
  EP::GUI::Window* window;
  EP::GUI::TextInput* fileName;
};



static void setUpBaseWindow(EP::GUI::Area* const parent) {
  // Editor:
  {
    EP::GUI::Window* window = new EP::GUI::Window("window",parent,50,50,200,300,"Options");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);

    EP::GUI::Button* be = new EP::GUI::Button("buttonEditor",window,x+5,y+5,90,20,"new Editor");
    be->onClickIs([parent]() {
      Editor* e = new Editor(parent);
    });
  }
}


