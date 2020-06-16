#include "../evp/gui.hpp"
#include "../evp/voronoi.hpp"

typedef int CInfo;

class MapArea : public evp::GUI::Area{
public:
  MapArea(evp::GUI::Area* const parent,
          const int nPoints,
	  const float dx,
	  const float dy
	 )  : Area("mapArea",parent,0,0,dx,dy) {
     vmap = new evp::VoronoiMap<CInfo>(10000,dx,dy);
  }
  virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
    float gx = x_+px*pscale;
    float gy = y_+py*pscale;
    DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);
    vmap->draw(gx,gy,zoom_,target);
  }
  void zoomIs(const float z) {
    zoom_ = std::min(1e2f, std::max(1e-2f,z));
    sizeIs(vmap->spread_x*zoom_, vmap->spread_y*zoom_);
    evp::GUI::ScrollArea* a = dynamic_cast<evp::GUI::ScrollArea*>(parent()->parent());
    a->adjustChildren();// make sure scrollers adjust
  }
  virtual void onMouseWheelScrolled(const float dx, const float dy, const float x, const float y) {
    evp::GUI::ScrollArea* a = dynamic_cast<evp::GUI::ScrollArea*>(parent()->parent());
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
     ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
      float gx = globalX();
      float gy = globalY();
      float oldZ = zoom_;
      zoomIs(pow(1.1,dy) * zoom_);
      a->adjustChildren();// make sure scrollers adjust
      
      float realZF = zoom_/oldZ;
      a->doScroll(
		  (x-gx)*(realZF-1),
		  (y-gy)*(realZF-1)
		  );// take zoom correction, to warp around mouse
      
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
     ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
      a->doScroll(-dy*10,-dx*10);// invert scroll axes
    } else {
      a->doScroll(-dx*10,-dy*10);
    }
  }
private:
  evp::VoronoiMap<CInfo>* vmap;
  float zoom_=1.0;
  bool hasData_=false;
};

class Editor {
public:
  Editor(evp::GUI::Area* const parent) {
    window = new evp::GUI::Window("editor",parent,200,200,300,300,"Editor");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);
 
    fileName = new evp::GUI::TextInput("fileNameInput",window,x,y,200,20,"file.txt");

    evp::GUI::Button* loadb = new evp::GUI::Button("buttonLoad",window,x+205,y,50,20,"Load");
    evp::GUI::Button* storeb = new evp::GUI::Button("buttonStore",window,x+265,y,50,20,"Store");
    loadb->onClickIs([this]() {
      load();
    });
    storeb->onClickIs([this]() {
      store();
    });

    mapArea = new MapArea(NULL,10000,1000,1000);
    evp::GUI::Area* scroll = new evp::GUI::ScrollArea("scroll",window,mapArea,x,y+100,dx,dy);
    mapArea->colorIs(evp::Color(1,0,0));
    scroll->fillParentIs(true,true);
  }
  void load() {std::cout << "**LOAD**\n";}
  void store() {std::cout << "**STORE**\n";}
private:
  evp::GUI::Window* window;
  evp::GUI::TextInput* fileName;
  MapArea* mapArea;
};



static void setUpBaseWindow(evp::GUI::Area* const parent) {
  // Editor:
  {
    evp::GUI::Window* window = new evp::GUI::Window("window",parent,50,50,200,300,"Options");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);

    evp::GUI::Button* be = new evp::GUI::Button("buttonEditor",window,x+5,y+5,90,20,"new Editor");
    be->onClickIs([parent]() {
      Editor* e = new Editor(parent);
    });
  }
}


