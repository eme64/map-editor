#ifndef EVP_GUI_HPP
#define EVP_GUI_HPP
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <list>
#include <functional>
#include <iomanip> // setprecision
#include <sstream> // stringstream


namespace evp {
  class Function {
  public:
    virtual double fwd(double const x) {return std::min(1.0,std::max(0.0,x));}
    virtual double bwd(double const y) {return std::min(1.0,std::max(0.0,y));}
  };
  class FunctionLin : public Function {
  public:
    FunctionLin(double const v0,double const v1) :v0_(v0),v1_(v1) {}
    virtual double fwd(double const x) {double const tmp = std::min(1.0,std::max(0.0,x)); return v0_+tmp*(v1_-v0_);}
    virtual double bwd(double const y) {double const tmp = (y-v0_)/(v1_-v0_); return std::min(1.0,std::max(0.0,tmp));}
  private:
    double v0_,v1_;
  };
  class FunctionExp : public Function {
  public:
    FunctionExp(double const v0,double const v1) :v0_(v0),v1_(v1) {}
    virtual double fwd(double const x) {
      double const tmp = std::min(1.0,std::max(0.0,x));
      return std::pow(v1_/v0_,tmp)*v0_;
    }
    virtual double bwd(double const y) {
      double const tmp = std::log(y/v0_)/std::log(v1_/v0_);
      return std::min(1.0,std::max(0.0,tmp));
    }
  private:
    double v0_,v1_;
  };
  class Color {
  public:
    float r,g,b,a;//0..1
    Color(const float r,const float g,const float b,const float a=1.0) {
      this->r = std::min(1.0f,std::max(0.0f,r));
      this->g = std::min(1.0f,std::max(0.0f,g));
      this->b = std::min(1.0f,std::max(0.0f,b));
      this->a = std::min(1.0f,std::max(0.0f,a));
    }
    Color() : Color(0,0,0) {}
    Color(const sf::Color c) : Color(c.r/255.0,c.g/255.0,c.b/255.0,c.a/255.0) {}

    Color operator*(const float scale) {return Color(scale*r,scale*g,scale*b,a);}
    Color operator+(const Color &c) {return Color(r+c.r,g+c.r,b+c.b,a+c.a);}
    Color operator-(const Color &c) {return Color(r-c.r,g-c.g,b-c.b,a-c.a);}

    sf::Color toSFML() const {
      return sf::Color(255.0*r,255.0*g,255.0*b,255.0*a);
    }
  protected:
  };
  Color ColorHue(float hue);
  void ColorToHSV(const Color c, float &hue, float &v, float &s);
  Color HSVToColor(const float hue, const float v, const float s);
  // sf::Color HueToRGB(float hue) {
  //   hue = fmod(hue,1.0);
  //   hue*=6.0;
  //   if (hue<1.0) {
  //     return sf::Color(255.0,255.0*hue,0);
  //   }
  //   hue-=1.0;
  //   if (hue<1.0) {
  //     return sf::Color(255.0-255.0*hue,255.0,0);
  //   }
  //   hue-=1.0;
  //   if (hue<1.0) {
  //     return sf::Color(0,255.0,255.0*hue);
  //   }
  //   hue-=1.0;
  //   if (hue<1.0) {
  //     return sf::Color(0,255.0-255.0*hue,255.0);
  //   }
  //   hue-=1.0;
  //   if (hue<1.0) {
  //     return sf::Color(255.0*hue,0,255.0);
  //   }
  //   hue-=1.0;
  //   if (hue<1.0) {
  //     return sf::Color(255.0,0,255.0-255.0*hue);
  //   }
  //   return sf::Color(255,255,255);
  // }
  //
  class Font {
  public:
    static sf::Font font;
    static bool isFont;
    static sf::Font& getFont() {
      if (!isFont) {
        if (!font.loadFromFile("extern/arial.ttf")) {
            std::cout << "font could not be loaded!" << std::endl;
        } else {
          isFont = true;
        }
      }
      return font;
    }
  };

  float DrawText(float x, float y, std::string text, float size, sf::RenderTarget &target, const Color& color,float alignX=0,float alignY=0);

  void DrawRect(float x, float y, float dx, float dy, sf::RenderTarget &target, const Color& color);
  
  void DrawOval(float x, float y, float dx, float dy, sf::RenderTarget &target, const Color& color);

  void DrawLine(float x1, float y1, float x2, float y2, sf::RenderTarget &target, const Color& color,float width=1);

  void DrawTriangle(float x0, float y0, float x1, float y1,float x2, float y2, sf::RenderTarget &target, const Color& color);

  class ViewAnchor {
  public:
    ViewAnchor(sf::RenderTarget &target,const float gx,const float gy,const float cx,const float cy,const float cdx,const float cdy) : target_(target) {//restrict / intersect current
      viewOld_ = target_.getView();
      sf::Vector2u size = target_.getSize();
      sf::FloatRect rect = sf::FloatRect((gx+cx)/size.x,(gy+cy)/size.y,(cdx)/size.x,(cdy)/size.y);
      sf::FloatRect inter;
      rect.intersects(viewOld_.getViewport(),inter);
      sf::View view;
      view.setViewport(inter);
      view.reset(sf::FloatRect(inter.left*size.x,inter.top*size.y,inter.width*size.x,inter.height*size.y));
      target_.setView(view);
    }
    ViewAnchor(sf::RenderTarget &target) : target_(target) { // jailbreak
      viewOld_ = target_.getView();
      sf::Vector2u size = target_.getSize();
      sf::View view;
      view.setViewport(sf::FloatRect(0,0,1,1));
      view.reset(sf::FloatRect(0,0,size.x,size.y));
      target_.setView(view);
    }
    ~ViewAnchor() {// pop stack
      target_.setView(viewOld_);
    }
  protected:
    sf::RenderTarget &target_;
    sf::View viewOld_;
  };


  // void DrawDot(float x, float y, sf::RenderWindow &window, sf::Color color = sf::Color(255,0,0))
  // {
  //   float r = 3;
  //   sf::CircleShape shape(r);
  //   shape.setFillColor(color);
  //   shape.setPosition(x-r, y-r);
  //   window.draw(shape, sf::BlendAdd);
  // }
  //
  // void DrawLine(float x1, float y1, float x2, float y2, sf::RenderWindow &window)
  // {
  //   sf::Vertex line[] =
  //   {
  //     sf::Vertex(sf::Vector2f(x1,y1), sf::Color(0,0,0)),
  //     sf::Vertex(sf::Vector2f(x2,y2), sf::Color(255,255,255))
  //   };
  //   window.draw(line, 2, sf::Lines, sf::BlendAdd);
  // }
  //




  namespace GUI {
    // base: Area
    // Windows,Buttons,expanders(x,y-sliders)

    class Area {
    public:
      Area(const std::string& name,Area* const parent, const float x,const float y,const float dx,const float dy)
      : name_(name),parent_(parent),x_(x),y_(y),dx_(dx),dy_(dy){
        if (parent_) {parent_->childIs(this);}
      }

      float x() const {return x_;}
      float y() const {return y_;}
      float dx() const {return dx_;}
      float dy() const {return dy_;}
      virtual float scale() const {return 1;}
      float globalScale() const {return scale()*(parent_?parent_->globalScale():1);}
      virtual float globalX() const {return x_*globalScale()+(parent_?parent_->globalX():0);}
      virtual float globalY() const {return y_*globalScale()+(parent_?parent_->globalY():0);}
      std::string name() const {return name_;}
      std::string fullName() const {
        if (parent_) {
          return parent_->fullName()+"/"+name_;
        }else{return name_;
        }
      }

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        // draw relative to parent scale (pscale)
        float gx = x_+px*pscale;
        float gy = y_+py*pscale;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx, gy,target,pscale);
        }
      }

      virtual void childSize(float &cdx, float &cdy) {
        cdx = dx_;
        cdy = dy_;
      }
      virtual void childOffset(float &cx, float &cy) {
        cx = 0;
        cy = 0;
      }

      void onResizeParent() {
        if (parent_) {
          float cdx,cdy,cx,cy;
          parent_->childSize(cdx,cdy);
          parent_->childOffset(cx,cy);
          if (fillParentX_&&parent_) {
            if(fillParentWithOffset_) {
	      sizeIs(cdx-x_+cx,dy_);
	    } else {
	      sizeIs(cdx,dy_);
              positionIs(cx,y_);
	    }
	  }
	  if (fillParentY_&&parent_) {
            if(fillParentWithOffset_) {
	      sizeIs(dx_,cdy-y_+cy);
	    } else {
	      sizeIs(dx_,cdy);
              positionIs(x_,cy);
	    }
	  }
	}
      }

      virtual void onMouseOverStart() {}
      virtual void onMouseOver(const float px,const float py,const float pscale) {}// relative to parent
      virtual void onMouseOverEnd() {}

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        std::cout << "onMouseDownStart " << fullName() << " fist:" << std::to_string(isFirstDown) << std::endl;
        if (isFirstDown) {setFocus();}
        return false;
      }
      //isFirstDown: true if just mouseDown, false if slided from other area that did not lock drag.
      // return: capture drag. can only capture if isFreshDown==true

      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {return true;}
      // px,py: relative to parent, position of mouse in last frame.
      //dx/dy the moving the mouse is trying to do. If change (eg =0), mouse position will be changed accordingly.
      //return true: keep drag, false: chain mouse

      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        std::cout << "onMouseDownEnd " << fullName() << " captured:" << std::to_string(isCaptured) << " last:" << std::to_string(isLastDown) << std::endl;
      }
      // isLastDown: if true indicates that mouse physically released, else only left scope bc not captured

      virtual Area* checkMouseOver(const float px,const float py,const float pscale,const bool doNotify=true) {// relative to parent
        if (px>=x_*pscale and py>=y_*pscale and px<=(x_+dx_)*pscale and py<=(y_+dy_)*pscale) {
          for (auto &c : children_) {
            Area* over = c->checkMouseOver(px-x_*pscale,py-y_*pscale,pscale*scale(),doNotify);
            if (over!=NULL) {
              return over;
            }
          }
          if (doNotify) {onMouseOver(px,py,pscale);}
          return this;
        }
        return NULL;
      }

      virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
        std::cout << "onKeyPressed " << fullName() << " focus:" << std::to_string(keyCode) << std::endl;
      }

      Area* sizeIs(const float dx,const float dy) {
        const float dxOld = dx_;
        const float dyOld = dy_;
        dx_=dx; dy_=dy;
        onResize(dxOld,dyOld);
        for (auto &c : children_) {
          c->onResizeParent();
        }
        return this;
      }
      
      // dx,dy: scroll units
      // x,y: global mouse coordinates
      virtual void onMouseWheelScrolled(const float dx, const float dy, const float x, const float y) {
        //std::cout << "Scrolled " << fullName() << " by: " << dx << " " << dy << "\n";
      }
      
      virtual void onResize(const float dxOld, const float dyOld) {
        //std::cout << "resizig: " << fullName() << std::endl;
      }

      Area* positionIs(const float x,const float y) {
        const float xOld = x_;
        const float yOld = y_;
        x_=x; y_=y;
        onPosition(xOld,yOld);
        return this;
      }
      virtual void onPosition(const float xOld, const float yOld) {
        //std::cout << "position: " << fullName() << std::endl;
      }

      Area* fillParentIs(bool const fillX, bool const fillY, bool const withOffset) {
        fillParentX_ = fillX;
        fillParentY_ = fillY;
	fillParentWithOffset_ = withOffset;
        if (fillParentX_ || fillParentY_) {onResizeParent();}
        return this;}
      Area* childIs(Area* c) {children_.push_back(c); return this;}
      void childDel(Area* c) {children_.remove(c);}
      Area* parentIs(Area* p) {parent_=p; return this;}
      Area* parent() {return parent_;}
      Area* firstChild() {return children_.front();}
      bool isFirstChild() {return parent_?this==parent_->firstChild():true;}
      void doDelete(bool const doNotify = true) {
        if (isDeleted_) {return;}
        isDeleted_ = true;

        if (doNotify) {
          goDeleteNotify();
        }

        std::cout << "checkFocus" << std::endl;
        if (isFocus()) {unFocus();}
        std::cout << "doDelete: " << fullName() << std::endl;
        std::list<Area*> cCopy = children_;
        // cannot use original, bc is modified by children
        for (auto &c : cCopy) {
          std::cout << "inc" << c->fullName() << std::endl;
          c->doDelete(false);
          std::cout << "outc" << std::endl;
        }
        std::cout << "rm from parent: " << fullName() << std::endl;
        if (parent_) {parent_->childDel(this); parent_=NULL;}
        std::cout << "delete: " << fullName() << std::endl;
        delete this;
        std::cout << "done." << std::endl;
      }

      void goDeleteNotify() {
        // goes down the tree, recursively
        //processing each node for delete notification
        // post-order
        for (auto &c : children_) {
          c->goDeleteNotify();
        }

        onDeleteNotify(this);
      }
      void onDeleteIs(std::function<void(Area*)> f) {onDelete_.push_back(f);}
      void onDeleteNotify(Area* const a) {
        // runs up the tree, notifies on each step if required
        for (auto &f : onDelete_) {f(a);}
        if (parent_) {
          parent_->onDeleteNotify(a);
        }
      }
      void doDeleteChildren() {
        std::cout << "doDeleteChildren: " << fullName() << std::endl;
        std::list<Area*> cCopy = children_;
        // cannot use original, bc is modified by children
        for (auto &c : cCopy) {
          std::cout << "inc" << c->fullName() << std::endl;
          c->doDelete(true);
          std::cout << "outc" << std::endl;
        }
      }
      void setFocus(bool isRecursive=false) {
        //std::cout << "setFocus " << fullName() << std::endl;
        if (!isRecursive) {
          Area* const oldFocus = getFocus();
          if (oldFocus==this) {return;}
          if (oldFocus) {oldFocus->unFocus();}
          isFocus_=true;
	  onSetFocus();
        }
        isFocusPath_=true;
        if (parent_) {
          parent_->firstChildIs(this);
          parent_->setFocus(true);
        }
      }
      void unFocus() {
        //std::cout << "unFocus " << fullName() << std::endl;
	if(isFocus_ && !isDeleted_) {onUnFocus();}
        isFocus_=false;
        isFocusPath_=false;
        if (parent_) {parent_->unFocus();}
      }
      Area* getFocus(bool traverseDown=false) {
        // travels up to top parent, then travels down until hits the focus child
        if (isFocus_) {return this;}

        if (traverseDown) {
          Area* const f = children_.front();
          if (f) {
            return f->getFocus(true);
          } else {
            return NULL;
          }
        } else {
          if (parent_) {
            parent_->getFocus(false);
          } else {
            return getFocus(true);
          }
        }
      }
      bool isFocus() {return isFocus_;}
      bool isFocusPath() {return isFocusPath_;}
      
      virtual void onSetFocus() {}
      virtual void onUnFocus() {}

      void colorIs(Color c) {bgColor_=c;}

    protected:
      std::string name_;
      Area* parent_ = NULL;
      std::list<Area*> children_;
      bool firstChildIs(Area* c) {
        const bool found = (std::find(children_.begin(), children_.end(), c) != children_.end());
        if (found) {children_.remove(c);}
        children_.push_front(c);
        return found;
      }
      float x_,y_,dx_,dy_; // x,y relative to parent
      bool fillParentX_ = false;
      bool fillParentY_ = false;
      bool fillParentWithOffset_=false;
      Color bgColor_ = Color(0.05,0.05,0.1);
      bool isFocus_=false;
      bool isFocusPath_=false;
      std::vector<std::function<void(Area*)>> onDelete_;
      bool isDeleted_ = false;
    };// class Area
    class Label : public Area {
    public:
      Label(const std::string& name,Area* const parent,
             const float x,const float y,const float fontSize,
             const std::string text,
             const Color textColor = Color(0,0,0)
            )
      : Area(name,parent,x,y,1,1),text_(text),textColor_(textColor),fontSize_(fontSize){}

      virtual Area* checkMouseOver(const float px,const float py,const float pscale,const bool doNotify=true) {// relative to parent
        return NULL;
      }
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        DrawText(gx+1, gy+1, text_, fontSize_*pscale, target, textColor_,allignX_,allignY_);
      }
      void textIs(std::string text) {text_=text;}
      void allignIs(const float x,const float y) {allignX_=x;allignY_=y;}
    protected:
      std::string text_;
      float fontSize_;
      Color textColor_;
      float allignX_=0;
      float allignY_=0;
    };
    class Button : public Area {
    public:
      Button(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             const std::string text,
             const std::vector<Color> bgColors = std::vector<Color>{Color(0.5,0.5,0.5),Color(0.4,0.4,0.4),Color(0.3,0.3,0.3),Color(0.2,0.2,0.2)},
             const std::vector<Color> textColors = std::vector<Color>{Color(0,0,0),Color(0,0,0),Color(1,1,1),Color(1,1,1)}
            )
      : Area(name,parent,x,y,dx,dy),text_(text),bgColors_(bgColors),textColors_(textColors){}

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColors_[state_]);
        DrawText(gx+1, gy+1, text_, (dy_-2)*pscale, target, textColors_[state_]);
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx,gy,target,pscale);
        }
      }

      virtual void onMouseOverStart() {if (state_==0) {state_=1;}}
      virtual void onMouseOver(const float px,const float py) {// relative to parent
        if (state_==0) {state_=1;}
      }
      virtual void onMouseOverEnd() {if (state_==1) {state_=0;}}

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        std::cout << "onMouseDownStart " << fullName() << " fist:" << std::to_string(isFirstDown) << std::endl;
        if (isFirstDown) {
          setFocus();
          if(onClick_) {onClick_();}
        }
        return true;
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {dx=0,dy=0;return false;}

      void onClickIs(std::function<void()> f) {onClick_=f;}
      void buttonColorIs(const std::vector<Color> bgColors) {bgColors_=bgColors;}
    protected:
      std::string text_;
      std::vector<Color> bgColors_,textColors_;
      size_t state_=0;//0:normal, 1:mouse over, 2:pressing, 3:clicked
      std::function<void()> onClick_;
    };
    class Knob : public Area {
    public:
      Knob(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             Function* func = NULL,
             const Color bgColor = Color(0,0,0),
             const Color knobColor = Color(0.5,0.5,0.5),
             const Color textColor = Color(1,1,1)
            )
      : Area(name,parent,x,y,dx,dy),textColor_(textColor),knobColor_(knobColor){
        if (func==NULL) {func = new Function();}
        func_=func;
        colorIs(bgColor);
      }
      ~Knob() {delete func_;}

      void knobColorIs(const Color _knobColor) {knobColor_=_knobColor;}

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        float centerX = gx+dx_*0.5*pscale;
        float centerY = gy+dy_*0.5*pscale;
        float r = std::min(dx_,dy_)*0.5*pscale;
        DrawOval(centerX-r,centerY-r, r*2,r*2,target,bgColor_);
        float angle0 = M_PI*2*0.3;
        float angle2 = M_PI*2*1.2;
        double val = func_->fwd(value_);
        float angle1 = angle0+value_*(angle2-angle0);
        size_t nAngle = 30;
        float dAngle = (angle1-angle0)/(float)nAngle;
        for (size_t i = 0; i < nAngle; i++) {
          DrawTriangle(
            centerX,centerY,
            centerX+r*std::cos(angle0+i*dAngle),centerY+r*std::sin(angle0+i*dAngle),
            centerX+r*std::cos(angle0+(i+1)*dAngle),centerY+r*std::sin(angle0+(i+1)*dAngle),
            target, knobColor_
          );
        }
        DrawLine(centerX, centerY, centerX+r*std::cos(angle0), centerY+r*std::sin(angle0), target, textColor_,1);
        DrawLine(centerX, centerY, centerX+r*std::cos(angle1), centerY+r*std::sin(angle1), target, textColor_,1);
        DrawLine(centerX, centerY, centerX+r*std::cos(angle2), centerY+r*std::sin(angle2), target, textColor_,1);
        float innerR = r*0.8;
        DrawOval(centerX-innerR,centerY-innerR, innerR*2,innerR*2,target,bgColor_);
        std::stringstream ss;
        int l = std::min(state_?7.0:5.0,std::max(0.0,(state_?5.0:3.0)-std::log10(val)));
        ss << std::fixed << std::setprecision(l) << val;
        std::string mystring = ss.str();
        DrawText(centerX,centerY, mystring, r*0.6, target, textColor_,0.5,0.5);
      }

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
          knobPan = 0;
          state_ = true;
        }
        return true;
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        knobPan+=dx;
        double const knobD = std::pow(10,std::min(1.5,std::max(-2.0,knobPan/100.0)))*0.0001;
        value_-=dy*knobD;
        value_ = std::min(1.0,std::max(0.0,value_));
        if(onValue_) {onValue_(func_->fwd(value_));}
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        state_=false;
      }
      double value() {return func_->fwd(value_);}
      void valueIs(double const v) {
        value_ = func_->bwd(v);
        if(onValue_) {onValue_(v);}
      }
      void onValueIs(std::function<void(float)> onVal) {onValue_=onVal;}// use to listen to value change
    protected:
      Color textColor_,knobColor_;
      double value_ = 0.5;
      double knobPan = 0;
      Function* func_;
      std::function<void(double)> onValue_;
      bool state_ = false;
    };

    class AreaDraw : public Area {
    public:
      AreaDraw(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             const Color bgColor = Color(0,0,0)
            )
      : Area(name,parent,x,y,dx,dy){
        colorIs(bgColor);
      }
      
      // capturing draw
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        float dx_s = dx_*pscale;
        float dy_s = dy_*pscale;
        DrawRect(gx, gy, dx_s,dy_s, target, bgColor_);
        if (onDraw_) {onDraw_(gx,gy,dx_,dy_,pscale,target);}
      }
      void onDrawIs(std::function<void(float,float,float,float,float,sf::RenderTarget&)> _onDraw) {onDraw_=_onDraw;}
    
      // capturing mouseDown
      typedef std::function<bool(const bool, const float, const float)> onMouseDownStart_f;
      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if(onMouseDownStart_) {return onMouseDownStart_(isFirstDown,x,y);}
	return false;
      }
      void onMouseDownStartIs(onMouseDownStart_f f) {onMouseDownStart_ = f;}
      
      typedef std::function<bool(const bool, const float, const float,float&,float&,Area* const over)> onMouseDown_f;
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        if(onMouseDown_) {return onMouseDown_(isCaptured,x,y,dx,dy,over);}
	return true;// keep drag
      }
      void onMouseDownIs(onMouseDown_f f) {onMouseDown_ = f;}

      typedef std::function<void(const bool, const bool, const float, const float,Area* const over)> onMouseDownEnd_f;
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        if(onMouseDownEnd_) {return onMouseDownEnd_(isCaptured,isLastDown,x,y,over);}
      }
      void onMouseDownEndIs(onMouseDownEnd_f f) {onMouseDownEnd_ = f;}
    protected:
      std::function<void(float,float,float,float,float,sf::RenderTarget&)> onDraw_;//gx,gy,dx,dy,scale,target
      onMouseDownStart_f onMouseDownStart_;
      onMouseDown_f onMouseDown_;
      onMouseDownEnd_f onMouseDownEnd_;
    };
    
    class TextInput : public Area {
    public:
      TextInput(const std::string& name,Area* const parent,
		const float x,const float y,const float dx,const float dy,
		const std::string text,
                const std::vector<Color> bgColors = std::vector<Color>{Color(0.5,0.5,0.5),Color(0.4,0.4,0.4),Color(0.3,0.3,0.3),Color(0.2,0.2,0.2)},
                const std::vector<Color> textColors = std::vector<Color>{Color(0,0,0),Color(0,0,0),Color(1,1,1),Color(1,1,1)},
		const Color currsorColor = Color(0.5,0,0)
	       ) : Area(name,parent,x,y,dx,dy), text_(text), bgColors_(bgColors), textColors_(textColors), currsorColor_(currsorColor), currsorIdx(text_.size()){}
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        int state = isFocus()?2:(mouseOver_?1:0);
	DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColors_[state]);
	std::string p0 = text_.substr(0,currsorIdx);
	std::string p1 = text_.substr(currsorIdx);
	float tdx = DrawText(gx+1, gy+1, p0, (dy_-4)*pscale, target, textColors_[state]);
	if(isFocus()) {
	  DrawRect(gx+1+tdx,gy+1,1,16,target,currsorColor_);
	}
	DrawText(gx+1+tdx, gy+1, p1, (dy_-4)*pscale, target, textColors_[state]);
      }

      virtual void onMouseOverStart() {if (!mouseOver_) {mouseOver_=true;}}
      virtual void onMouseOverEnd() {if (mouseOver_) {mouseOver_=false;}}

      virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
        switch (keyCode) {
          case sf::Keyboard::Key::Left:{currsorMove(-1); break;}
          case sf::Keyboard::Key::Right:{currsorMove(1); break;}
          case sf::Keyboard::Key::Escape:{unFocus(); break;}
          case sf::Keyboard::Key::Return:{unFocus(); break;}
          case sf::Keyboard::Key::Delete:{currsorDel(); break;}
          case sf::Keyboard::Key::BackSpace:{currsorBack(); break;}
          case sf::Keyboard::Key::Period:{currsorPut(46); break;}
          case sf::Keyboard::Key::Comma:{currsorPut(44); break;}
          default: {
            if(keyCode >=0 && keyCode <=25) {
              if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
                 ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
		currsorPut(65+keyCode);
	      } else {
		currsorPut(97+keyCode);
	      }
	    } else if(keyCode >=26 && keyCode <=35) {
              currsorPut(48+keyCode-26);
	    }
	    break;}
        }
      }
      
      virtual void onUnFocus() {
        onTextCommit();
      }

      void currsorMove(const int move) {
        currsorIdx = std::min((int)text_.size(), std::max(0, currsorIdx + move));
      }
      void currsorDel() {
      	std::string p0 = text_.substr(0,currsorIdx);
	std::string p1 = text_.substr(std::min((int)text_.size(),currsorIdx+1));
	if(onText_) {onText_(text_);}
	text_=p0+p1;
      }
      void currsorBack() {
      	std::string p0 = text_.substr(0,std::max(0,currsorIdx-1));
	std::string p1 = text_.substr(currsorIdx);
	text_=p0+p1;
	if(onText_) {onText_(text_);}
	currsorIdx = p0.size();
      }
      void currsorPut(const char c) {
      	std::string p0 = text_.substr(0,currsorIdx);
	std::string p1 = text_.substr(currsorIdx);
	text_=p0+c+p1;
	if(onText_) {onText_(text_);}
	currsorIdx+=1;
      }
      
      void onTextCommit() {// call when leave context to let client know
	if(onTextCommit_) {onTextCommit_(text_);}
      }

      void textIs(const std::string &s) {
        text_ = s;
	currsorIdx = std::min((int)text_.size(), currsorIdx);
	if(onText_) {onText_(text_);}
      }

      std::string text() {return text_;}
      void onTextIs(std::function<void(const std::string&)> f) {onText_=f;}
      void onTextCommitIs(std::function<void(const std::string&)> f) {onTextCommit_=f;}

    protected:
      std::string text_;
      std::vector<Color> bgColors_,textColors_;
      Color currsorColor_;
      int currsorIdx;
      std::function<void(const std::string&)> onText_; // for each diff
      std::function<void(const std::string&)> onTextCommit_; // when leave context/onUnFocus
      bool mouseOver_ = false;
    };
 
    class ColorSlot : public Area {
    public:
      ColorSlot(const std::string& name,Area* const parent,
		const float x,const float y,const float dx,const float dy,
		const Color colorVal,
                const std::vector<Color> bgColors = std::vector<Color>{Color(0.2,0.2,0.2),Color(0.5,0.5,0.5),Color(1,1,1)}
	       ) : Area(name,parent,x,y,dx,dy), colorVal_(colorVal), bgColors_(bgColors) {}
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        int state = isFocus()?2:(mouseOver_?1:0);
	DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColors_[state]);
	DrawRect(gx+1, gy+1, dx_*pscale-2, dy_*pscale-2, target, colorVal_);
      }

      virtual void onMouseOverStart() {if (!mouseOver_) {mouseOver_=true;}}
      virtual void onMouseOverEnd() {if (mouseOver_) {mouseOver_=false;}}

      virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
        switch (keyCode) {
          case sf::Keyboard::Key::Escape:{unFocus(); break;}
          case sf::Keyboard::Key::C:{
	    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
             ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
	      clip_ = colorVal();
	    }
	    break;}
          case sf::Keyboard::Key::V:{
	    if(isPaste_) {
	      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
               ||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
	        colorValIs(clip_);
	      }
	    }
	    break;}
        }
      }
      void colorValIs(const Color val) {
        colorVal_ = val;
	if(onColor_) {onColor_(colorVal_);}
      }
      Color colorVal() {return colorVal_;}
      void onColorIs(std::function<void(const Color)> f) {onColor_=f;}
      
      void isPasteIs(const bool p) {isPaste_ = p;}

      static Color clip_;
    protected:
      Color colorVal_;
      std::vector<Color> bgColors_;
      std::function<void(const Color)> onColor_;
      bool mouseOver_ = false;
      bool isPaste_ = true;
    };
    
    // set up window where one can pick a color
    void makeColorPickerWindow(Area* const parent, const float x, const float y);
    
    class Window : public Area {
    public:
      Window(const std::string& name,Area* const parent, const float x,const float y,const float dx,const float dy, const std::string title)
      : Area(name,parent,x,y,dx,dy),title_(title){
        bgColor_ = Color(0.45,0.4,0.4);
        closeButton_ = new Button("close",this,borderSize,borderSize,headerSize-2*borderSize,headerSize-2*borderSize,"X",
                                        std::vector<Color>{Color(0.5,0,0),Color(0.4,0,0),Color(0.2,0,0),Color(0.2,0,0)},
                                        std::vector<Color>{Color(1,0.5,0.5),Color(1,0.8,0.8),Color(0.6,0.1,0.1),Color(0.5,0,0)}
                                      );
      }

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {// px global pos of parent
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;

        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_*(isFocusPath()?1.0:0.8));
        DrawText(gx+borderSize+headerSize, gy+borderSize, title_, (headerSize-2*borderSize)*pscale, target, Color(1,1,1));

        {
          float cx,cy,cdx,cdy;childSize(cdx,cdy);childOffset(cx,cy);
          ViewAnchor viewAnchor(target,gx,gy,cx,cy,cdx,cdy);

          for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
            if (closeButton_!=(*rit)) {
              (*rit)->draw(gx,gy,target,pscale);
            }
          }
        }

        closeButton_->draw(gx,gy,target,pscale);
      }

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
          float lx = x-globalX();
          float ly = y-globalY();

          // check what to do with drag:
          resetResizing();
          if (lx<borderSize) {isResizingLeft = true;}
          if (ly<borderSize) {isResizingTop = true;}
          if (lx>dx_-borderSize) {isResizingRight = true;}
          if (ly>dy_-borderSize) {isResizingBottom = true;}
          isDragging = !(isResizingLeft || isResizingTop || isResizingRight || isResizingBottom);
        }
        return true;
      }

      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        float moveX=0, moveY=0,moveDX=0, moveDY=0;
        float cx,cy,cdx,cdy;
        parent_->childSize(cdx,cdy);parent_->childOffset(cx,cy);

        if (isCaptured) {
          if (isDragging || isResizingLeft) {
            moveX=std::max(cx-x_,std::min(cx+cdx-x_-dx_,dx));
            dx=moveX;
            if (isResizingLeft) {moveDX=-dx;}
          }
          if (isDragging || isResizingTop) {
            moveY=std::max(cy-y_,std::min(cy+cdy-y_-dy_,dy));
            dy=moveY;
            if (isResizingTop) {moveDY=-dy;}
          }
          if (isResizingRight) {
            moveDX=std::max(cx-x_,std::min(cx+cdx-x_-dx_,dx));
            dx=moveDX;
          }
          if (isResizingBottom) {
            moveDY=std::max(cy-y_,std::min(cy+cdy-y_-dy_,dy));
            dy=moveDY;
          }
        }
        if (moveX!=0 || moveY!=0) {positionIs(x_+moveX,y_+moveY);}
        if (moveDX!=0 || moveDY!=0) {sizeIs(dx_+moveDX,dy_+moveDY);}
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        resetResizing();
        isDragging = false;
      }

      virtual void childSize(float &cdx, float &cdy) {
        cdx = dx_-2*borderSize;
        cdy = dy_-headerSize-borderSize;
      }
      virtual void childOffset(float &cx, float &cy) {
        cx = borderSize;
        cy = headerSize;
      }

    protected:
      std::string title_;
      Button* closeButton_=NULL;//shortcut, still in children
      const float borderSize = 5.0; // sides and bottom
      const float headerSize = 25.0;// top
      bool isResizable = false;
      bool isResizingLeft = false;
      bool isResizingRight = false;
      bool isResizingTop = false;
      bool isResizingBottom = false;
      bool isDragging = false;
      void resetResizing() {
        isResizingLeft = false;
        isResizingRight = false;
        isResizingTop = false;
        isResizingBottom = false;
      }
    };

    class Slider : public Area {
    public:
      class SliderButton : public Area {
      public:
        SliderButton(const std::string& name,Area* const parent,
               const float x,const float y,const float dx,const float dy,
               const std::vector<Color> buttonColors
              )
        : Area(name,parent,x,y,dx,dy),buttonColors_(buttonColors){}
        virtual void onMouseOverStart() {if (state_==0) {state_=1;}}
        virtual void onMouseOver() {if (state_==0) {state_=1;}}
        virtual void onMouseOverEnd() {if (state_==1) {state_=0;}}

        virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
          if (isFirstDown) {
            setFocus();
            state_=2;
          }
          return isFirstDown;
        }
        virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
          if (isCaptured) {
            if(Slider* p = dynamic_cast<Slider*>(parent_)) {
              return p->onSliderButtonDrag(dx,dy);// let parent handle the case
            }
          }
          return true;
        }
        virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
          if (isCaptured) {
            state_=0;
          }
        }
        virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
          float gx = x_*pscale+px;
          float gy = y_*pscale+py;
          DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, buttonColors_[state_]);
          for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
            (*rit)->draw(gx, gy,target,pscale);
          }
        }
      protected:
        std::vector<Color> buttonColors_;
        size_t state_=0;//0:normal, 1:mouse over, 2:pressing
      };
      Slider(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             const bool isHorizontal,const float minVal,const float maxVal,const float initVal,const float buttonLength,
             const std::vector<Color> bgColors = std::vector<Color>{Color(0.2,0.2,0.2),Color(0.15,0.15,0.15),Color(0.1,0.1,0.1)},
             const std::vector<Color> buttonColors = std::vector<Color>{Color(0.3,0.3,0.3),Color(0.35,0.35,0.35),Color(0.4,0.4,0.4)}
            )
      : Area(name,parent,x,y,dx,dy),
        isHorizontal_(isHorizontal),minVal_(minVal),maxVal_(maxVal),val_(initVal),buttonLength_(buttonLength),
        bgColors_(bgColors){
            sliderButton_ = new SliderButton("button",this,0,0,dx/2,dy/2,buttonColors);
            adjustChildren();
      }
      void adjustChildren() {
        const float bLenRel = buttonLength_/(maxVal_-minVal_+buttonLength_);
        const float bPosRel = (val_-minVal_)/(maxVal_-minVal_+buttonLength_);
        if (isHorizontal_) {
          sliderButton_->sizeIs(dx_*bLenRel,dy_);
          sliderButton_->positionIs(dx_*bPosRel,0);
        } else {
          sliderButton_->sizeIs(dx_,dy_*bLenRel);
          sliderButton_->positionIs(0,dy_*bPosRel);
        }
      }
      double val() {return val_;}
      void valIs(float val) {
        val = std::min(maxVal_, std::max(minVal_, val));
	if (val_!=val) {
          val_=val;
          adjustChildren();
          if (onVal_) {onVal_(val_);}
        }
      }
      void onValIs(std::function<void(float)> onVal) {onVal_=onVal;}// use to listen to value change
      void valBoundsIs(const float minVal,const float maxVal,const float buttonLength) {
        minVal_=minVal; maxVal_=maxVal;buttonLength_=buttonLength;
        if (val_<minVal_) {valIs(minVal_);} else if (val_>maxVal_) {valIs(maxVal_);}
        adjustChildren();
      }
      bool onSliderButtonDrag(float &dx, float &dy) {
        if (minVal_==maxVal_) {return true;}// abort, want no bad action
        float *absDrag;
        float absLen;
        if (isHorizontal_) {
          absDrag = &dx;
          absLen = dx_-sliderButton_->dx();
        } else {
          absDrag = &dy;
          absLen = dy_-sliderButton_->dy();
        }
        float relDrag = (*absDrag)/(absLen);
        float wantDrag = relDrag*(maxVal_-minVal_);
        float doDrag = std::max(minVal_-val_,std::min(maxVal_-val_,wantDrag));
        valIs(val_+doDrag);// apply drag
        float doRelDrag = doDrag/(maxVal_-minVal_);
        *absDrag = doRelDrag*absLen;// feedback to caller
        return true;
      }
      virtual void onResize(const float dxOld, const float dyOld) {
        adjustChildren();
      }

      virtual void onMouseOverStart() {if (state_==0) {state_=1;}}
      virtual void onMouseOver() {if (state_==0) {state_=1;}}
      virtual void onMouseOverEnd() {if (state_==1) {state_=0;}}

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
          state_=2;
        }
        return isFirstDown;
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {return true;}
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        if (isCaptured) {
          state_=0;
        }
      }

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColors_[state_]);
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx, gy,target,pscale);
        }
      }

    protected:
      SliderButton* sliderButton_;
      bool isHorizontal_;
      float minVal_,maxVal_,val_,buttonLength_;
      std::function<void(float)> onVal_;
      std::vector<Color> bgColors_;
      size_t state_=0;//0:normal, 1:mouse over, 2:pressing
    };

    class ScrollArea : public Area {
    public:
      class ScrollAreaViewer : public Area {
      public:
        ScrollAreaViewer(const std::string& name,Area* const parent, Area* const child,const float x,const float y,const float dx,const float dy)
        : Area(name,parent,x,y,dx,dy),child_(child){
          child_->parentIs(this);
          childIs(child_);
        }
        virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
          float gx = x_*pscale+px;
          float gy = y_*pscale+py;
          DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);

          sf::View viewOld = target.getView(); // push new view
          sf::View view;
          float cx,cy,cdx,cdy;childSize(cdx,cdy);childOffset(cx,cy);
          sf::Vector2u size = target.getSize();
          sf::FloatRect rect = sf::FloatRect((gx+cx)/size.x,(gy+cy)/size.y,(cdx)/size.x,(cdy)/size.y);
          sf::FloatRect inter;
          rect.intersects(viewOld.getViewport(),inter);
          view.setViewport(inter);
          view.reset(sf::FloatRect(cx,cy,inter.width*size.x,inter.height*size.y));
          view.move(sf::Vector2f(gx,gy));
          target.setView(view);

          for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
              (*rit)->draw(gx+childOffsetX_,gy+childOffsetY_,target,pscale);
          }
          target.setView(viewOld);// pop new view
        }
        virtual Area* checkMouseOver(const float px,const float py,const float pscale,const bool doNotify=true) {// relative to parent
          if (px>=x_*pscale and py>=y_*pscale and px<=(x_+dx_)*pscale and py<=(y_+dy_)*pscale) {
            for (auto &c : children_) {
              Area* over = c->checkMouseOver(px-x_*pscale-childOffsetX_,py-y_*pscale-childOffsetY_,doNotify);
              if (over!=NULL) {
                return over;
              }
            }
            if (doNotify) {onMouseOver(px,py,pscale);}
            return this;
          }
          return NULL;
        }
        virtual float globalX() const {return x_+(parent_?parent_->globalX():0)+childOffsetX_;}
        virtual float globalY() const {return y_+(parent_?parent_->globalY():0)+childOffsetY_;}
        void childOffsetXIs(const float v) {childOffsetX_=v;}
        void childOffsetYIs(const float v) {childOffsetY_=v;}
        Area* child() const {return child_;}
      protected:
        Area* child_;
        float childOffsetX_=0;
        float childOffsetY_=0;
      };

      ScrollArea(const std::string& name,Area* const parent, Area* const child,
                 const float x,const float y,const float dx,const float dy,
                 const bool scrollX=true,const bool scrollY=true)
      : Area(name,parent,x,y,dx,dy),scrollX_(scrollX),scrollY_(scrollY){
        scrollView_ = new ScrollAreaViewer("viewer",this,child,0,0,dx,dy);
        if (scrollX_) {
          sliderX_ = new Slider("sliderX",this,0,0,dx,dy,true,0,100,0,20);
          sliderX_->onValIs([this](float val) {scrollView_->childOffsetXIs(-val);});
        }
        if (scrollY_) {
          sliderY_ = new Slider("sliderY",this,0,0,dx,dy,false,0,100,0,20);
          sliderY_->onValIs([this](float val) {scrollView_->childOffsetYIs(-val);});
        }
        adjustChildren();// above just set bogus values, this will adjust them all anyway
      }
      void adjustChildren() {
        float middleX = scrollX_?dx_-scrollerWidth_:dx_;
        float middleY = scrollX_?dy_-scrollerWidth_:dy_;
        scrollView_->sizeIs(middleX,middleY);
        if (sliderX_) {
          sliderX_->positionIs(0,middleY);
          sliderX_->sizeIs(middleX,scrollerWidth_);
          sliderX_->valBoundsIs(0.0f,std::max(0.0f,scrollView_->child()->dx()-middleX),middleX);
        }
        if (sliderY_) {
          sliderY_->positionIs(middleX,0);
          sliderY_->sizeIs(scrollerWidth_,middleY);
          sliderY_->valBoundsIs(0.0f,std::max(0.0f,scrollView_->child()->dy()-middleY),middleY);
        }
      }
      virtual void onResize(const float dxOld, const float dyOld) {
        adjustChildren();
      }

      void doScroll(const float dx, const float dy) {
        if(sliderX_) {
	  sliderX_->valIs(sliderX_->val() + dx);
	}
        if(sliderY_) {
	  sliderY_->valIs(sliderY_->val() + dy);
	}
      }
    protected:
      ScrollAreaViewer* scrollView_;
      Slider* sliderX_;
      Slider* sliderY_;
      const bool scrollX_;
      const bool scrollY_;
      float scrollerWidth_=20;
    };
     
    class TabArea : public Area {
    public:
      // has sub-areas, can be filled by content.
      TabArea(const std::string& name,Area* const parent, const float x,const float y,const float dx,const float dy,
                const std::vector<Color> tabColors = std::vector<Color>{Color(0.2,0.2,0.2),Color(0.3,0.3,0.3)},
                const std::vector<Color> textColors = std::vector<Color>{Color(0.8,0.8,0.8),Color(1,1,1)}
		      )
      : Area(name,parent,x,y,dx,dy), tabColors_(tabColors), textColors_(textColors) {}

      void addTab(Area* a, const std::string &name) {
	// Adds in an area, sets up parent/child structure.
	auto* tab = new Area("tab_"+std::to_string(tabs_.size()),this,0,20,100,100);
        tab->fillParentIs(true,true,true);
	a->parentIs(tab);
	tab->childIs(a);
        tabs_.push_back(tab);
	names_.push_back(name);
	tab->onResizeParent(); // make sure it is adjusted
      }
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        // draw relative to parent scale (pscale)
        float gx = x_+px*pscale;
        float gy = y_+py*pscale;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);
        
	float ddx = dx_*pscale / tabs_.size();
	for(int i=0;i<tabs_.size();i++) {
          int state = (selected_==i);
	  DrawRect(gx+i*ddx, gy, ddx, 20*pscale, target, tabColors_[state]);
	  DrawRect(gx+i*ddx+1, gy+1, ddx-2, 20*pscale-1, target, tabColors_[state]*0.6);
	  DrawText(gx+i*ddx+1, gy+5, names_[i], (12)*pscale, target, textColors_[state]);
	}
        
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx, gy,target,pscale);
        }
      }
      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
	  float px = x-globalX();
	  float ddx = dx_ / tabs_.size();
          int id = floor(px/ddx);
	  selected_ = id;
	  tabs_[id]->setFocus();
	  return true;
	}
        return false;
      }
 
    private:
      std::vector<Area*> tabs_;
      std::vector<std::string> names_;
      int selected_=0;
      std::vector<Color> tabColors_,textColors_;
    };
 
    class SplitArea : public Area {
    public:
      // has 2 sub areas, split by a %percent line.
      SplitArea(const std::string& name,Area* const parent, const float x,const float y,const float dx,const float dy, const bool horizontal)
      : Area(name,parent,x,y,dx,dy), horizontal_(horizontal) {
        colorIs(evp::Color(0.2,0.1,0.1));
	sub_.resize(2);
	sub_[0] = new Area("a0",this,0,0,10,10);
	sub_[1] = new Area("a1",this,0,0,10,10);
	factorIs(0.5);
      }
      
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        // draw relative to parent scale (pscale)
        float gx = x_+px*pscale;
        float gy = y_+py*pscale;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);
        
        if(horizontal_)  {
	  DrawRect(gx+2, gy+factor_*dy_, dx_*pscale-4, 1, target, bgColor_*0.5);
	} else {
	  DrawRect(gx+factor_*dx_, gy+2, 1, dy_*pscale-4, target, bgColor_*0.5);
	}
        
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx, gy,target,pscale);
        }
      }
      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
	  state_=1;
	  return true;
	}
        return false;
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        if (isCaptured) {
          onDragSeparator(dx,dy);
	}
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        if (isCaptured) {
          state_=0;
        }
      }

      void onDragSeparator(float &dx, float &dy) {
        if(horizontal_) {
	  float df = dy/dy_;
	  float oldF = factor_;
	  factorIs(factor_+df);
	  dy = (factor_ - oldF)*dy_;
	} else {
	  float df = dx/dx_;
	  float oldF = factor_;
	  factorIs(factor_+df);
	  dx = (factor_ - oldF)*dx_;
	}
      }
 
      void factorIs(float f) {
        f = std::min(0.9f,std::max(0.1f,f));
	factor_ = f;
        adjustChildren();
      }
      
      void adjustChildren() {
        if(horizontal_) {
	  sub_[0]->sizeIs(dx_,dy_*factor_-2);
          sub_[0]->positionIs(0,0);
          sub_[1]->sizeIs(dx_,dy_*(1-factor_)-2);
          sub_[1]->positionIs(0,dy_*factor_+2);
	} else {
	  sub_[0]->sizeIs(dx_*factor_-2,dy_);
          sub_[0]->positionIs(0,0);
          sub_[1]->sizeIs(dx_*(1-factor_)-2,dy_);
          sub_[1]->positionIs(dx_*factor_+2,0);
	}
      }

      virtual void onResize(const float dxOld, const float dyOld) {
        adjustChildren();
      }

      Area* sub(int i) {return sub_[i];}// attach children to this: 0,1

    private:
      std::vector<Area*> sub_;
      bool horizontal_;
      float factor_ = 0.5; // 0..1
      int state_ = 0; // 0: rest, 1: pressing/dragging
    };

    class Socket : public Area {
    public:
      enum Direction {Up,Down};
      Socket(const std::string& name,Area* const parent,
             const float x,const float y,Direction direction,
             const std::string text,
             const Color bgColor = Color(1,1,1,0),
             const std::vector<Color> textColor = std::vector<Color>{Color(0.5,0.5,0.5),Color(1,1,1)},
             const std::vector<Color> connectorColor = std::vector<Color>{Color(1,1,1),Color(0,0,0)}
            )
      : Area(name,parent,x,y,20,20),text_(text),textColor_(textColor),connectorColor_(connectorColor),direction_(direction){
        colorIs(bgColor);
        switch (direction_) {
          case Direction::Up:
          case Direction::Down:{
            sizeIs(35,15);
            break;
          }
        }
        canTakeSink = []() {return true;};
        canMakeSource = [](Socket* s) {return true;};
        onSinkIs = [](Socket* s) {};
        onSinkDel = [](Socket* s) {};
        onSourceIs = [](Socket* s) {};
        onSourceDel = [](Socket* s) {};

        onDeleteIs([this](Area* const a) {
          if (a==this) {
            sourceIs(NULL);
            auto tmpVec = sink();
            for (auto &s : tmpVec) {
              s->sourceIs(NULL);
            }
          }
        });
      }

      void textColorIs(const std::vector<Color> _textColor) {textColor_=_textColor;};
      void connectorColorIs(const std::vector<Color> _connectorColor) {connectorColor_=_connectorColor;};
      std::vector<Color> connectorColor() {return connectorColor_;}

      void canTakeSinkIs(std::function<bool()> f) {canTakeSink=f;}
      void canMakeSourceIs(std::function<bool(Socket*)> f) {canMakeSource=f;}
      void onSinkIsIs(std::function<void(Socket*)> f) {onSinkIs=f;}
      void onSinkDelIs(std::function<void(Socket*)> f) {onSinkDel=f;}
      void onSourceIsIs(std::function<void(Socket*)> f) {onSourceIs=f;}
      void onSourceDelIs(std::function<void(Socket*)> f) {onSourceDel=f;}

      float socketX() {
        switch (direction_) {
          case Direction::Up:return dx_*0.5*globalScale();
          case Direction::Down:return dx_*0.5*globalScale();
        }
      };
      float socketY() {
        switch (direction_) {
          case Direction::Up:return 0;//return dx_*0.5*globalScale();
          case Direction::Down:return dy_*globalScale();//return (dy_-dx_*0.5)*globalScale();
        }
      };

      // for client use
      void sourceIs(Socket* source) {
        if (source==source_) {return;}
        if (source_) {
          onSourceDel(source_);
          source_->sinkDel(this);
        }
        source_=source;
        if (source_) {
          source_->sinkIs(this);
          onSourceIs(source_);
        }
      }

      std::list<Socket*>& sink() {return sink_;}

      // internal: only react!
      void sinkIs(Socket* sink) {
        const bool found = (std::find(sink_.begin(), sink_.end(), sink) != sink_.end());
        if (!found) {
          sink_.push_front(sink);
          onSinkIs(sink);
        }
      }
      void sinkDel(Socket* sink) {
        const bool found = (std::find(sink_.begin(), sink_.end(), sink) != sink_.end());
        if (found) {
          onSinkDel(sink);
          sink_.remove(sink);
        }
      }

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        const float gx = x_*pscale+px;
        const float gy = y_*pscale+py;
        const float dx_s = dx_*pscale;
        const float dy_s = dy_*pscale;

        switch (direction_) {
          case Direction::Up:{
            // DrawOval(gx, gy, dx_s,dx_s,target,bgColor_);
            // DrawRect(gx, gy+dx_s*0.5, dx_s, dy_s-dx_s*0.5, target, bgColor_);
            // DrawOval(gx+dx_s*0.25, gy+dx_s*0.25, dx_s*0.5,dx_s*0.5,target,socketColor_);
            DrawRect(gx, gy, dx_s, dy_s, target, bgColor_);
            DrawText(gx+dx_s*0.5, gy+1, text_, dy_s*0.9, target, textColor_[not mouseOver_],0.5,0);
            break;
          }
          case Direction::Down:{
            // DrawOval(gx, gy+dy_s-dx_s, dx_s,dx_s,target,bgColor_);
            // DrawRect(gx, gy, dx_s, dy_s-dx_s*0.5, target, bgColor_);
            // DrawOval(gx+dx_s*0.25, gy+dy_s-dx_s*0.75, dx_s*0.5,dx_s*0.5,target,socketColor_);
            // DrawText(gx+1, gy+1, text_, (dy_s-dx_s), target, textColor_);
            DrawRect(gx, gy, dx_s, dy_s, target, bgColor_);
            DrawText(gx+1, gy+1, text_, dy_s*0.9, target, textColor_[not mouseOver_]);
            break;
          }
        }
        const float sx = gx+socketX();
        const float sy = gy+socketY();
        if (source_) {
          const float ox = source_->globalX()+source_->socketX();
          const float oy = source_->globalY()+source_->socketY();
          DrawLine(sx,sy,ox,oy,target,connectorColor_[1],5);
          DrawLine(sx,sy,ox,oy,target,connectorColor_[0],3);
        }
        for (std::list<Socket*>::reverse_iterator rit=sink_.rbegin(); rit!=sink_.rend(); ++rit) {
          const float ox = (*rit)->globalX()+(*rit)->socketX();
          const float oy = (*rit)->globalY()+(*rit)->socketY();
          DrawLine(sx,sy,ox,oy,target,(*rit)->connectorColor()[1],5);
          DrawLine(sx,sy,ox,oy,target,(*rit)->connectorColor()[0],3);
        }
        if (isSettingSource) {
          DrawLine(sx,sy,isSettingSourceX_,isSettingSourceY_,target,connectorColor_[1],5);
          DrawLine(sx,sy,isSettingSourceX_,isSettingSourceY_,target,connectorColor_[0],3);
        }

        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx, gy,target,pscale);
        }
      }
      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {
          setFocus();
          if (canTakeSink()) {
            isSettingSource = true;
            isSettingSourceX_ = x;
            isSettingSourceY_ = y;
            sourceIs(NULL);
            return true;
          } else {
            return false;
          }
        }
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        if (isCaptured and isSettingSource) {
          isSettingSourceX_ = x;
          isSettingSourceY_ = y;
        }
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        isSettingSource = false;
        if (isCaptured) {
          Socket* source = dynamic_cast<Socket*>(over);
          if (source and source!=this and source!=source_ and source->canMakeSource(this)) {
            sourceIs(source);
          }
        }
      }

      virtual void onMouseOverStart() {mouseOver_=true;}
      virtual void onMouseOver(const float px,const float py,const float pscale) {}// relative to parent
      virtual void onMouseOverEnd() {mouseOver_=false;}

    protected:
      std::string text_;
      std::vector<Color> textColor_, connectorColor_;
      bool mouseOver_=false;
      Direction direction_;
      Socket* source_=NULL;

      bool isSettingSource = false;
      float isSettingSourceX_,isSettingSourceY_;

      std::list<Socket*> sink_;

      // requests:
      std::function<bool()> canTakeSink;// return if allow
      std::function<bool(Socket*)> canMakeSource;// return if allow

      // reactions:
      std::function<void(Socket*)> onSinkIs;
      std::function<void(Socket*)> onSinkDel;
      std::function<void(Socket*)> onSourceIs;
      std::function<void(Socket*)> onSourceDel;
    };

    class Block : public Area {
    public:
      Block(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             const Color bgColor = Color(0.5,0.5,0.5)
            )
      : Area(name,parent,x,y,dx,dy){
        colorIs(bgColor);
      }
      virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
        switch (keyCode) {
          case sf::Keyboard::Key::Delete:{doDelete(); break;}
        }
      }
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        if (onDraw_) {onDraw_();}
        const float gx = px+x_*pscale;
        const float gy = py+y_*pscale;

        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_*(isFocus()?1.0:0.4));
        DrawRect(gx+2, gy+2, dx_*pscale-4, dy_*pscale-4, target, bgColor_*0.7);
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx,gy,target,pscale);
        }
      }

      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {setFocus();}
        return true;
      }

      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        float moveX=0, moveY=0;
        float cx,cy,cdx,cdy;
        parent_->childSize(cdx,cdy);parent_->childOffset(cx,cy);

        if (isCaptured) {
          moveX=std::max(cx-x_,std::min(cx+cdx-x_-dx_,dx/globalScale()));
          dx=moveX*globalScale();
          moveY=std::max(cy-y_,std::min(cy+cdy-y_-dy_,dy/globalScale()));
          dy=moveY*globalScale();
        }
        if (moveX!=0 || moveY!=0) {positionIs(x_+moveX,y_+moveY);}
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
      }
      void onDrawIs(std::function<void()> onDraw) {onDraw_=onDraw;}
    protected:
      std::function<void()> onDraw_;
    };

    class BlockHolder : public Area {
    public:
      BlockHolder(const std::string& name,Area* const parent,
                  const float x,const float y,const float dx,const float dy
                 )
      : Area(name,parent,x,y,dx,dy){}
      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        const float gx = px+x_*pscale;
        const float gy = py+y_*pscale;

        DrawRect(gx,gy, dx_*pscale, dy_*pscale, target, bgColor_*0);
        const float tileD = 100.0*pscale*scale_;
        for (size_t x = 0; x < dx_/tileD+1; x++) {
          for (size_t y = 0; y < dy_/tileD+1; y++) {
            DrawRect(gx+x*tileD, gy+y*tileD, tileD-2, tileD-2, target, bgColor_);
          }
        }

        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx,gy,target,pscale*scale_);
        }
      }
      virtual float scale() const {return scale_;}
      void scaleMult(const float _factor) {
        scale_*=_factor;
        std::cout << "scaleMult: " << scale_ << std::endl;
      }
      virtual void onKeyPressed(const sf::Keyboard::Key keyCode) {
        switch (keyCode) {
          case sf::Keyboard::Key::Subtract:{scaleMult(0.9); break;}
          case sf::Keyboard::Key::Add:{scaleMult(1.1); break;}
        }
      }
    protected:
      float scale_ = 1.0;
    };


    class BlockTemplate : public Area {
      // drag template from this area to blockHolder area. Instantiate a new Block.
    public:
      BlockTemplate(const std::string& name,Area* const parent,
             const float x,const float y,const float dx,const float dy,
             const std::string text,
             const Color bgColor = Color(0.5,0.5,0.5),
             const Color textColor = Color(0,0,0)
            )
      : Area(name,parent,x,y,dx,dy),text_(text),textColor_(textColor) {
        colorIs(bgColor);
        doInstantiate_ = [this](BlockHolder* bh,const float x,const float y) {
          std::cout << "not implemented: adding " << this->fullName() << " to " << bh->fullName() << std::endl;
        };
      }
      void doInstantiateIs(std::function<void(BlockHolder*,const float,const float)> f) {doInstantiate_=f;}

      virtual void draw(const float px,const float py, sf::RenderTarget &target, const float pscale) {
        float gx = x_*pscale+px;
        float gy = y_*pscale+py;
        DrawRect(gx, gy, dx_*pscale, dy_*pscale, target, bgColor_);
        DrawText(gx+1, gy+1, text_, (dy_-2)*pscale, target, textColor_);
        if (isDraggingTemplate1_) {
          ViewAnchor viewAnchor(target);// jailbreak
          DrawRect(isDraggingTemplateX_, isDraggingTemplateY_, dx_*pscale, dy_*pscale, target, bgColor_);
          DrawText(isDraggingTemplateX_+1, isDraggingTemplateY_+1, text_, (dy_-2)*pscale, target, textColor_);
        }
        for (std::list<Area*>::reverse_iterator rit=children_.rbegin(); rit!=children_.rend(); ++rit) {
          (*rit)->draw(gx,gy,target,pscale);
        }
      }
      virtual bool onMouseDownStart(const bool isFirstDown,const float x,const float y) {
        if (isFirstDown) {setFocus();}
        isDraggingTemplate1_ = true;
        isDraggingTemplateX_ = x;
        isDraggingTemplateY_ = y;
        return true;
      }
      virtual bool onMouseDown(const bool isCaptured,const float x,const float y,float &dx, float &dy,Area* const over) {
        if (isCaptured and isDraggingTemplate1_) {
          isDraggingTemplateX_ = x;
          isDraggingTemplateY_ = y;
        }
        return true;
      }
      virtual void onMouseDownEnd(const bool isCaptured, const bool isLastDown,const float x,const float y,Area* const over) {
        if (isCaptured && isDraggingTemplate1_) {
          BlockHolder* bh = dynamic_cast<BlockHolder*>(over);
          if (bh) {
            doInstantiate_(bh,x-bh->globalX(),y-bh->globalY());
          }
        }
        isDraggingTemplate1_ = false;
      }
    protected:
      std::string text_;
      Color textColor_;

      bool isDraggingTemplate1_ = false;
      float isDraggingTemplateX_,isDraggingTemplateY_;

      // requests:
      std::function<void(BlockHolder*,const float,const float)> doInstantiate_;
    };

    class MasterWindow {
    public:
      MasterWindow(const size_t dx,const size_t dy,std::string title,const bool fullScreen=false) {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;

        if (fullScreen) {
          renderWindow_ = new sf::RenderWindow(sf::VideoMode(dx, dy), title, sf::Style::Fullscreen, settings);
        } else {
          renderWindow_ = new sf::RenderWindow(sf::VideoMode(dx, dy), title, sf::Style::Default, settings);
        }

        renderWindow_->setVerticalSyncEnabled(true);
        renderWindow_->setKeyRepeatEnabled(false);

        sf::Vector2u size = renderWindow_->getSize();
        mainArea_ = (new evp::GUI::Area("main",NULL,0,0,size.x,size.y))->fillParentIs(true,true,false);
        mainArea_->onDeleteIs([this](Area* const a) {
          forgetArea(a);
        });

        sf::Vector2i mousepos = sf::Mouse::getPosition(*renderWindow_);
        lastMouseX_ = mousepos.x;
        lastMouseY_ = mousepos.y;
      }
      sf::RenderWindow* target() {return renderWindow_;}
      Area* area() {return mainArea_;}

      void forgetArea(Area* const a) {
        if (mouseDownArea_==a) {mouseDownReset();}
        if(mouseOverArea_==a){mouseOverIs(NULL);}
      }

      void mouseOverIs(Area* const mouseOverNew) {
        if (mouseOverArea_!=mouseOverNew) {
          // end old:
          if (!mouseDownCaptured_ && mouseDownArea_!=NULL && mouseDownArea_!=mouseOverNew) {
            mouseDownArea_->onMouseDownEnd(mouseDownCaptured_,false,lastMouseX_,lastMouseY_,mouseOverArea_);
            mouseDownArea_=NULL;
          }
          if (mouseOverArea_!=NULL) {
            mouseOverArea_->onMouseOverEnd();
            mouseOverArea_=NULL;
          }
          // start new:
          if (mouseOverNew!=NULL) {
            mouseOverArea_ = mouseOverNew;
            mouseOverArea_->onMouseOverStart();

            if (mouseDown_ && !mouseDownArea_) {
              mouseDownArea_ = mouseOverArea_;
              mouseDownArea_->onMouseDownStart(false,lastMouseX_,lastMouseY_);
              mouseDownCaptured_ = false;
            }
          }
        }
      }

      void update() {
        sf::Event event;
        while (renderWindow_->pollEvent(event)) {
          switch (event.type) {
            case sf::Event::Closed: {
              close();
              break;
            }
            case sf::Event::MouseWheelScrolled: {
              if (mouseOverArea_) {
	        int delta = event.mouseWheelScroll.delta;
	        int msx = event.mouseWheelScroll.x;
	        int msy = event.mouseWheelScroll.y;
                
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                  mouseOverArea_->onMouseWheelScrolled(0,delta,msx,msy);
		} else if(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel){
                  mouseOverArea_->onMouseWheelScrolled(delta,0,msx,msy);
	        }
	      }
              break;
            }
            case sf::Event::MouseButtonPressed: {
              if(event.mouseButton.button == sf::Mouse::Left) {
                mouseDown_ = true;
                if (mouseOverArea_) {
                  mouseDownArea_ = mouseOverArea_;
                  mouseDownCaptured_ = mouseOverArea_->onMouseDownStart(true,lastMouseX_,lastMouseY_);
                  //if (mouseDownCaptured_) {
                  //  std::cout << "mouseDownEvent captured by " << mouseOverArea_->fullName() << std::endl;
                  //}
                }
              } else if(event.mouseButton.button == sf::Mouse::Right) {
                // forget for now
              }
              break;
            }
            case sf::Event::MouseButtonReleased: {
              if(event.mouseButton.button == sf::Mouse::Left) {
                mouseDown_ = false;
                if (mouseDownArea_) {
                  mouseDownArea_->onMouseDownEnd(mouseDownCaptured_,true,lastMouseX_,lastMouseY_,mouseOverArea_);
                }
                mouseDownReset();
              } else if(event.mouseButton.button == sf::Mouse::Right) {
                // forget for now
              }
              break;
            }
            case sf::Event::MouseMoved: {
              float mouseX = event.mouseMove.x;
              float mouseY = event.mouseMove.y;
              const float origMouseDX = mouseX-lastMouseX_+mouseDownKeepDragDX_;
              const float origMouseDY = mouseY-lastMouseY_+mouseDownKeepDragDY_;
              float mouseDX = origMouseDX;
              float mouseDY = origMouseDY;

              bool keepDrag = true;

              if (mouseDown_) {
                // captured -> simply notify, update mouse
                // not captured: if has area: check, if leaves: update mouse, notify leaving, possibly start notify
                // no area: update mouse, possibly start notify after
                if (mouseDownArea_) {
                  keepDrag = mouseDownArea_->onMouseDown(mouseDownCaptured_,lastMouseX_,lastMouseY_,mouseDX,mouseDY,mouseOverArea_);
                  if (keepDrag) {
                    mouseDownKeepDragDX_+= origMouseDX-mouseDX;
                    mouseDownKeepDragDY_+= origMouseDY-mouseDY;
                  }
                }
              }

              if (keepDrag) {
                lastMouseX_+=origMouseDX;
                lastMouseY_+=origMouseDY;
              } else {
                lastMouseX_+=mouseDX;
                lastMouseY_+=mouseDX;
              }


              if (!(origMouseDX==mouseDX && origMouseDY==mouseDY) && !keepDrag) {
                sf::Mouse::setPosition(sf::Vector2i(lastMouseX_,lastMouseY_),*renderWindow_);
              }

              evp::GUI::Area* mouseOverNew = mainArea_->checkMouseOver(lastMouseX_,lastMouseY_,1.0);
              mouseOverIs(mouseOverNew);
              break;
            }
            case sf::Event::KeyPressed: {
              Area* const focus = mainArea_->getFocus();
              if (focus) {focus->onKeyPressed(event.key.code);}
              break;
            }
            case sf::Event::Resized: {
              sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
              renderWindow_->setView(sf::View(visibleArea));
              sf::Vector2u size = renderWindow_->getSize();
              mainArea_->sizeIs(size.x,size.y);
              break;
            }
            default: {break;}
          }
        }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        //  close();
        //}
      }
      void draw() {
        renderWindow_->clear();
        mainArea_->draw(0,0,*renderWindow_,1);
        // DrawDot(MOUSE_X, MOUSE_Y, window, sf::Color(255*MOUSE_RIGHT_DOWN,255*MOUSE_LEFT_DOWN,255));
        if (mouseOverArea_) {
          evp::DrawText(5,5, "over: " + mouseOverArea_->fullName(), 10, *renderWindow_, evp::Color(1.0,1.0,1.0));
        }
        if(mouseDown_) {
          std::string p = "down: " + (mouseDownArea_?mouseDownArea_->fullName():"None") + " " + (mouseDownCaptured_?"captured":"glide");
          evp::DrawText(5,20, p, 10, *renderWindow_, evp::Color(1.0,1.0,1.0));
        }
        {
          Area* const focus = mainArea_->getFocus();
          std::string p = "focus: " + (focus?focus->fullName():"None");
          evp::DrawText(5,35, p, 10, *renderWindow_, evp::Color(1.0,1.0,1.0));
        }

        renderWindow_->display();
      }
      void close() {renderWindow_->close();}
      bool isAlive() {return renderWindow_->isOpen();}
    protected:
      sf::RenderWindow *renderWindow_;
      Area* mainArea_;
      Area* mouseOverArea_;
      bool mouseDown_ = false;
      Area* mouseDownArea_;
      bool mouseDownCaptured_ = false;
      float mouseDownKeepDragDX_ = 0;
      float mouseDownKeepDragDY_ = 0;
      void mouseDownReset() {
        mouseDownArea_ = NULL;
        mouseDownCaptured_ = false;
        mouseDownKeepDragDX_ = 0;
        mouseDownKeepDragDY_ = 0;
      }
      float lastMouseX_;
      float lastMouseY_;
    };
  }// namespace GUI
}// namespace evp
#endif //EVP_GUI_HPP
