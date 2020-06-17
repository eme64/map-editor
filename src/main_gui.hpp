#include "../evp/gui.hpp"
#include "../evp/voronoi.hpp"

struct CInfo{
   int paletteId = 0;
   float colFac = 0;
};

class PaletteArea : public evp::GUI::Area{
public:
  PaletteArea(evp::GUI::Area* const parent)
	  : Area("paletteArea",parent,0,0,200,400) {
    colorIs(evp::Color(0.05,0.05,0.05));
    pname[1] = "defalut";
    pcolor[1] = evp::Color(1,1,1);
    repopulate();
  }
  void repopulate() {
    // remove all children, repopulate
    doDeleteChildren();
    
    float ypos = 0;
    // add all palette items
    for(auto it : pname) {
      int id = it.first;
      
      // label, color slot
      evp::GUI::Label* l = new evp::GUI::Label("label_"+std::to_string(id),
		                              this,2,2+ypos,10,std::to_string(id),
					      evp::Color(1,1,1)
		                              );
      evp::GUI::ColorSlot* s = new evp::GUI::ColorSlot("colorSlot_"+std::to_string(id),
       	                                      this,40,ypos+2,100,20,
       					      pcolor[id]);
      s->onColorIs([this,id](const evp::Color c) {
	pcolor[id] = c;
	onUpdate();
      });

      // text field
      auto ti = new evp::GUI::TextInput("nameInput_"+std::to_string(id),this,
		                        30,ypos+25,150,20,it.second);
      ti->onTextIs([id,this](std::string s){
        pname[id] = s;
	onUpdate();
      });

      // delete button
      evp::GUI::Button* delb = new evp::GUI::Button("delB_"+std::to_string(id),
		      this,2,ypos+27,16,16,"-");
      delb->onClickIs([this,id,delb]() {
        pname.erase(id);
        pcolor.erase(id);
        repopulate();
	onUpdate();
      });
 
      // background
      evp::GUI::AreaDraw* da = new evp::GUI::AreaDraw("bg_"+std::to_string(id), this,
            	                                  0,ypos,200,50);

      da->onDrawIs([id,this](float x, float y, float dx, float dy, float scale, sf::RenderTarget& target){
        if(selectedId_ == id) {
	  evp::DrawRect(x, y, dx, dy, target, evp::Color(1,1,1));
	  evp::DrawRect(x+1, y+1, dx-2, dy-2, target, evp::Color(0,0,0));
	} else {
	  evp::DrawRect(x, y, dx, dy, target, evp::Color(0.2,0.2,0.2));
	  evp::DrawRect(x+1, y+1, dx-2, dy-2, target, evp::Color(0,0,0));
	}
      });
      da->onMouseDownStartIs([id,this,da](const bool isFirstDown, const float x, const float y){
        if(isFirstDown) {
	  selectedId_ = id;
	}
	return false;
      });

      ypos+=50;
    }

    // add plus button
    evp::GUI::Button* plusb = new evp::GUI::Button("buttonPlus",this,2,ypos+2,16,16,"+");
    plusb->onClickIs([this]() {
      addNewItem();
    });
    ypos+=20;
    sizeIs(200,ypos);
  }
  void addNewItem() {
    std::cout << "addNewItem\n";
    int idMax = 0;
    for(auto it : pname) {
      idMax = std::max(idMax, it.first);
    }
    
    int id = idMax+1;
    pname[id] = "new";
    pcolor[id] = evp::Color(0,0,0);

    repopulate();
    onUpdate();
  }
  
  void onUpdate() {
    if(onUpdate_) {onUpdate_();}
  }
  void onUpdateIs(std::function<void()> f) {onUpdate_=f;}
  
  evp::Color paletteColor(int id) {
    if(pcolor.find(id)!=pcolor.end()) {
      return pcolor[id];
    } else {
      return evp::Color(1,1,1,(float)rand()/(RAND_MAX)*0.2);
    }
  }

private:
  std::map<int,std::string> pname;
  std::map<int,evp::Color> pcolor;
  int selectedId_=0;
  std::function<void()> onUpdate_;
};


class MapArea : public evp::GUI::Area{
public:
  MapArea(evp::GUI::Area* const parent,
	  PaletteArea* paletteArea,
          const int nPoints,
	  const float dx,
	  const float dy
	 )  : Area("mapArea",parent,0,0,dx,dy),paletteArea_(paletteArea) {
     vmap = new evp::VoronoiMap<CInfo>(10000,dx,dy);
     mapInitialize();
     mapColorize();

     paletteArea_->onUpdateIs([this](){
       mapColorize();
     });
  }
  void mapInitialize() {
    // set CInfo for all cells:
    for(auto &c : vmap->cells) {
      c.info.paletteId = 1;
      c.info.colFac = 1 - ((float)rand()/(RAND_MAX))*0.2;
    }
  }
  void mapColorize() {
    // set colors according to new data
    for(auto &c : vmap->cells) {
      c.color = (paletteArea_->paletteColor(c.info.paletteId) * c.info.colFac).toSFML();
    }
    vmap->create_mesh();
  }
  virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
    float gx = x_+px*pscale;
    float gy = y_+py*pscale;
    DrawRect(gx, gy, dx_*pscale, dy_*pscale, target,
		    evp::HSVToColor((float)rand()/(RAND_MAX),0.5,0.5));
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
  PaletteArea* paletteArea_;
};


class Editor {
public:
  Editor(evp::GUI::Area* const parent) {
    window = new evp::GUI::Window("editor",parent,200,200,300,300,"Editor");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);
    
    // options on top
    fileName = new evp::GUI::TextInput("fileNameInput",window,x,y,200,20,"file.txt");

    evp::GUI::Button* loadb = new evp::GUI::Button("buttonLoad",window,x+205,y,50,20,"Load");
    evp::GUI::Button* storeb = new evp::GUI::Button("buttonStore",window,x+265,y,50,20,"Store");
    loadb->onClickIs([this]() {
      load();
    });
    storeb->onClickIs([this]() {
      store();
    });
    
    int topBarOffset = 100;
    
    // palette on left side
    paletteArea = new PaletteArea(NULL);
    evp::GUI::Area* scrollp = new evp::GUI::ScrollArea("scrollp",window,paletteArea,x,y+topBarOffset,220,dy);
    scrollp->fillParentIs(false,true,true);

    // MapArea in center / bottom-right
    mapArea = new MapArea(NULL,paletteArea,10000,1000,1000);
    evp::GUI::Area* scroll = new evp::GUI::ScrollArea("scroll",window,mapArea,x+225,y+topBarOffset,dx,dy);
    mapArea->colorIs(evp::Color(1,0,0));
    scroll->fillParentIs(true,true,true);
  }
  void load() {std::cout << "**LOAD**\n";}
  void store() {std::cout << "**STORE**\n";}
private:
  evp::GUI::Window* window;
  evp::GUI::TextInput* fileName;
  MapArea* mapArea;
  PaletteArea* paletteArea;
};



static void setUpBaseWindow(evp::GUI::Area* const parent) {
  // Editor:
  {
    evp::GUI::Window* window = new evp::GUI::Window("window",parent,50,50,200,300,"Options");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);

    evp::GUI::Button* be = new evp::GUI::Button("buttonEditor",window,x+5,y+5,180,20,"new Editor");
    be->onClickIs([parent]() {
      Editor* e = new Editor(parent);
    });

    evp::GUI::Button* bc = new evp::GUI::Button("buttonCP",window,x+5,y+30,180,20,"new Color Picker");
    bc->onClickIs([parent]() {
      evp::GUI::makeColorPickerWindow(parent, 100,100);
    });
 
  }
}


