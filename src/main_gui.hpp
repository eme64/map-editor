#ifndef EVP_MAIN_GUI_HPP
#define EVP_MAIN_GUI_HPP

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream> // stringstream

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
  void save(std::ofstream &myfile);

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

  long selectedId() {
    if(pcolor.find(selectedId_)!=pcolor.end()) {
      return selectedId_;
    } else {
      return -1;
    }
  }

  void selectNext() {
    if(selectedId()<=0 && pname.size()>0) {
      selectedId_ = pname.begin()->first;
    } else if(pname.size()>0) {
      bool found = false;
      int id = selectedId();
      for(auto it : pname) {
	if(found) {
	  selectedId_ = it.first;
	  return;
	} else if(it.first == id) {
	  found = true;
	}
        selectedId_ = pname.begin()->first;
      }
    }
  }
  void selectPrev() {
    if(selectedId()<=0 && pname.size()>0) {
      selectedId_ = pname.begin()->first;
    } else if(pname.size()>0) {
      int last = pname.rbegin()->first;
      int id = selectedId();
      for(auto it : pname) {
	if(it.first == id) {
	  selectedId_ = last;
	  return;
	}
	last = it.first;
      }
    }
  }

  void select(int pid) {
    if(pcolor.find(pid)!=pcolor.end()) {
      selectedId_ = pid;
    }
  }
  
  void clear() {
    pname.clear();
    pcolor.clear();
    selectedId_=0;
    onUpdate();
  }

  void addItem(const int id, const std::string &name, const evp::Color col) {
    pname[id] = name;
    pcolor[id] = col;
    onUpdate();
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
  void load(const std::string& fileName);
  void save(const std::string& fileName);

  void mapInitialize() {
    wantMapColorize_ = false;
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
    if(wantMapColorize_) {
      mapColorize();
    }
    vmap->draw(gx,gy,zoom_,target);
    
    if(cellOver_>=0) {
      evp::VoronoiMapCell<CInfo> &cell = vmap->cells[cellOver_];
      for(int i=0;i<cell.corners.size();i++) {
        int i2 = (i+1) % cell.corners.size();
	evp::Point &p1 = cell.corners[i];
	evp::Point &p2 = cell.corners[i2];
	evp::DrawLine(
	   gx + zoom_*p1.x,
	   gy + zoom_*p1.y,
	   gx + zoom_*p2.x,
	   gy + zoom_*p2.y,
	   target, evp::Color(1,1,1,1) - evp::Color(cell.color) + evp::Color(0,0,0,1)
	   );
      }
    }
    if(mode_==1) {
      // draw
      for(int cc : cellsSelected_) {
        evp::VoronoiMapCell<CInfo> &cell = vmap->cells[cc];
        for(int i=0;i<cell.corners.size();i++) {
          int i2 = (i+1) % cell.corners.size();
          evp::Point &p1 = cell.corners[i];
          evp::Point &p2 = cell.corners[i2];
          evp::DrawLine(
             gx + zoom_*p1.x,
             gy + zoom_*p1.y,
             gx + zoom_*p2.x,
             gy + zoom_*p2.y,
             target, evp::Color(1,1,1,1) - evp::Color(cell.color) + evp::Color(0,0,0,1)
             );
        }
      }
    }

    if(!showHUD_ && parent() && parent()->parent()) {
      float xx = parent()->parent()->globalX();
      float yy = parent()->parent()->globalY();
      evp::DrawRect(xx+2,yy+2,40,35,target,evp::Color(0,0,0,0.7));
      evp::DrawText(xx+5,yy+5,"[H]",12,target,evp::Color(1,1,1));
      evp::DrawText(xx+5,yy+20,modeText[mode_],12,target,evp::Color(1,1,1));
    } else {
      float xx = parent()->parent()->globalX();
      float yy = parent()->parent()->globalY();
      evp::DrawRect(xx+2,yy+2,300,300,target,evp::Color(0,0,0,0.7));
      evp::DrawText(xx+5,yy+5,"[H] hide HUD",12,target,evp::Color(1,1,1));
      evp::DrawText(xx+5,yy+20,modeText[mode_],12,target,evp::Color(1,1,1));
      evp::DrawText(xx+5,yy+35,"[M]ove",12,target,evp::Color(1,1,1));
      evp::DrawText(xx+5,yy+50,"[D]raw - [Alt + scrol] pen-size",12,target,evp::Color(1,1,1));
      evp::DrawText(xx+5,yy+65,"[S]elect",12,target,evp::Color(1,1,1));
      
      evp::DrawText(xx+5,yy+100,"[P + up/down] Palette Selection",12,target,evp::Color(1,1,1));
      

    }
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
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)
     ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt)) {
      drawRadiusIs(drawRadius_ * pow(1.1,-dy));
      float gx = globalX();
      float gy = globalY();
      float vx = (x-gx)/zoom_;
      float vy = (y-gy)/zoom_;
      updateCellSelection(vx,vy,cellOver_);
    } else {
      a->doScroll(-dx*10,-dy*10);
    }
  }

  virtual void onMouseOver(const float px,const float py,const float pscale) {
    float vx = px/zoom_;
    float vy = py/zoom_;
    size_t cid = vmap->getCell(vx,vy,0);
    cellOver_ = cid;

    if(mode_==1) {// in Draw mode:
      updateCellSelection(vx,vy,cid);
    }
  }
  void updateCellSelection(float x, float y, size_t cellId) {
    vmap->getCellsInRadius(x,y,drawRadius_ ,cellId,cellsSelected_);
  }

  virtual void onMouseOverEnd() {
    cellOver_ = -1;
    cellsSelected_.clear();
  }

  virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
    if (isFirstDown) {
      setFocus();
      float dx=0,dy=0;
      doAction(dx,dy);
      return true; // capture
    }
    return false;
  }

  virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
    if(isCaptured) {doAction(dx,dy);}
    return true;
  }
  
  void doAction(float &dx, float &dy) {
    switch(mode_) {
      case 0: {
        // Move
	if(dx!=0 || dy!=0) {
          evp::GUI::ScrollArea* a = dynamic_cast<evp::GUI::ScrollArea*>(parent()->parent());
	  a->doScroll(-dx,-dy);
	}
      break;}
      case 1: { // Draw
        long pid = paletteArea_->selectedId();
        if(pid>=0) {
	  if(cellOver_>=0) {
            evp::VoronoiMapCell<CInfo> &cell = vmap->cells[cellOver_];
            cell.info.paletteId = pid;
            wantMapColorize_=true;
          } 
          for(int cc : cellsSelected_) {
            evp::VoronoiMapCell<CInfo> &cell = vmap->cells[cc];
            cell.info.paletteId = pid;
            wantMapColorize_=true;
	  } 
	}
	break;}
      case 2: {
        // Select
        if(cellOver_>=0) {
          evp::VoronoiMapCell<CInfo> &cell = vmap->cells[cellOver_];
	  paletteArea_->select(cell.info.paletteId);
	}
      break;}
    }
  }

  virtual void onUnFocus() {mode_=0;} // make sure mode is reset

  virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
    switch (keyCode) {
      case sf::Keyboard::Key::H:{showHUD_ = !showHUD_; break;}
      case sf::Keyboard::Key::Down:{
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
	  paletteArea_->selectNext();
	}
      break;}
      case sf::Keyboard::Key::Up:{
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
	  paletteArea_->selectPrev();
	}
      break;}
      case sf::Keyboard::Key::M:{mode_ = 0; break;}
      case sf::Keyboard::Key::D:{mode_ = 1; break;}
      case sf::Keyboard::Key::S:{mode_ = 2; break;}
    }
  }
  
  void drawRadiusIs(float r) {
    drawRadius_ = std::min(1e3f,std::max(1.f,r));
    std::cout << drawRadius_ << "\n";
  }

private:
  evp::VoronoiMap<CInfo>* vmap;
  float zoom_=1.0;
  float drawRadius_=100.0;
  bool hasData_=false;
  PaletteArea* paletteArea_;
  size_t cellOver_ = -1;
  std::vector<size_t> cellsSelected_;
  bool wantMapColorize_ = false;

  // Command options:
  bool showHUD_ = false;
  int mode_ = 0;
  std::vector<std::string> modeText = {
    "Move",   // 0
    "Draw",   // 1
    "Select",   // 1
  };
};


class Editor {
public:
  Editor(evp::GUI::Area* const parent) {
    window = new evp::GUI::Window("editor",parent,200,20,500,500,"Editor");
    float x,y,dx,dy;
    window->childSize(dx,dy);
    window->childOffset(x,y);
    
    // options on top
    fileName = new evp::GUI::TextInput("fileNameInput",window,x,y,200,20,"file.txt");

    evp::GUI::Button* loadb = new evp::GUI::Button("buttonLoad",window,x+205,y,50,20,"Load");
    evp::GUI::Button* saveb = new evp::GUI::Button("buttonSave",window,x+265,y,50,20,"Save");
    loadb->onClickIs([this]() {
      load();
    });
    saveb->onClickIs([this]() {
      save();
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
  void load() {
    mapArea->load(fileName->text());
  }
  void save() {
    mapArea->save(fileName->text());
  }
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


#endif // EVP_MAIN_GUI_HPP
