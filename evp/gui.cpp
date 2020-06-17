#include "gui.hpp"

sf::Font evp::Font::font;
bool evp::Font::isFont = false;

evp::Color evp::ColorHue(float hue) {
  hue = fmod(hue,1.0);
  hue*=6.0;
    if (hue<1.0) {
      return Color(1,1*hue,0);
    }
    hue-=1.0;
    if (hue<1.0) {
      return Color(1-1*hue,1,0);
    }
    hue-=1.0;
    if (hue<1.0) {
      return Color(0,1,1*hue);
    }
    hue-=1.0;
    if (hue<1.0) {
      return Color(0,1-1*hue,1);
    }
    hue-=1.0;
    if (hue<1.0) {
      return Color(1*hue,0,1);
    }
    hue-=1.0;
    if (hue<1.0) {
      return Color(1,0,1-1*hue);
    }
    return Color(1,1,1);
}


void evp::ColorToHSV(const evp::Color c, float &hue, float &v, float &s) {
  float V = std::max(c.r, std::max(c.g, c.b));
  float M = std::min(c.r, std::min(c.g, c.b));
  float S = (V==0)? 0 : (V-M)/V;
  float C = V-M;
  if(C==0.0) {
    hue = 0;
  } else if(V==c.r) {
    hue = 1.0/6 * (0 + (c.g-c.b)/C);
  } else if(V==c.g) {
    hue = 1.0/6 * (2 + (c.b-c.r)/C);
  } else if(V==c.b) {
    hue = 1.0/6 * (4 + (c.r-c.g)/C);
  }
  while(hue<0) {hue+=1.0;}
  hue = fmod(hue, 1.0);
  v = V;
  s = S;
}

evp::Color evp::HSVToColor(const float hue, const float v, const float s) {
  float c = v*s;
  float h = 360*hue;
  float x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
  float m = v - c;

  if (h >= 0.0 && h < 60.0) {
    return evp::Color(c + m, x + m, m);
  } else if (h >= 60.0 && h < 120.0) {
    return evp::Color(x + m, c + m, m);
  } else if (h >= 120.0 && h < 180.0) {
    return evp::Color(m, c + m, x + m);
  } else if (h >= 180.0 && h < 240.0) {
    return evp::Color(m, x + m, c + m);
  } else if (h >= 240.0 && h < 300.0) {
    return evp::Color(x + m, m, c + m);
  } else if (h >= 300.0 && h <= 360.0) {
    return evp::Color(c + m, m, x + m);
  } else {
    return evp::Color(m, m, m);
  }
}

evp::Color evp::GUI::ColorSlot::clip_ = Color(0,0,0);


float evp::DrawText(float x, float y, std::string text, float size, sf::RenderTarget &target, const Color& color,float alignX,float alignY) {
  sf::Text shape(text, evp::Font::getFont());
  shape.setCharacterSize(std::floor(size));
  shape.setFillColor(color.toSFML());
  sf::FloatRect bounds = shape.getLocalBounds();
  shape.setPosition(std::floor(x-alignX*bounds.width),std::floor(y-alignY*bounds.height));
  //shape.setStyle(sf::Text::Bold | sf::Text::Underlined);
  target.draw(shape, sf::BlendAlpha);//BlendAdd
  return bounds.width;
}

void evp::DrawRect(float x, float y, float dx, float dy, sf::RenderTarget &target, const evp::Color& color) {
  sf::RectangleShape rectangle;
  rectangle.setSize(sf::Vector2f(dx, dy));
  rectangle.setFillColor(color.toSFML());
  rectangle.setPosition(x, y);
  target.draw(rectangle, sf::BlendAlpha);//BlendAdd
}
void evp::DrawOval(float x, float y, float dx, float dy, sf::RenderTarget &target, const evp::Color& color) {
  sf::CircleShape shape(1);
  shape.setScale(dx*0.5,dy*0.5);
  shape.setFillColor(color.toSFML());
  shape.setPosition(x, y);
  target.draw(shape, sf::BlendAlpha);
}
void evp::DrawLine(float x1, float y1, float x2, float y2, sf::RenderTarget &target, const evp::Color& color,float width) {
  const float dist = std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
  const float rad = std::atan2(y1-y2,x1-x2)+0.5*M_PI;
  const float angle = rad*180.0/M_PI;
  sf::RectangleShape rectangle;
  rectangle.setSize(sf::Vector2f(width, dist));
  rectangle.setFillColor(color.toSFML());
  rectangle.setPosition(x1-std::cos(rad)*width*0.5, y1-std::sin(rad)*width*0.5);
  rectangle.setRotation(angle);
  target.draw(rectangle, sf::BlendAlpha);//BlendAdd
  DrawOval(x1-width*0.5,y1-width*0.5,width,width,target,color);
  DrawOval(x2-width*0.5,y2-width*0.5,width,width,target,color);
}
void evp::DrawTriangle(float x0, float y0, float x1, float y1,float x2, float y2, sf::RenderTarget &target, const evp::Color& color) {
  sf::ConvexShape convex;
  convex.setPointCount(3);
  convex.setFillColor(color.toSFML());
  convex.setPoint(0, sf::Vector2f(x0,y0));
  convex.setPoint(1, sf::Vector2f(x1,y1));
  convex.setPoint(2, sf::Vector2f(x2,y2));
  target.draw(convex, sf::BlendAlpha);
}

void evp::GUI::makeColorPickerWindow(evp::GUI::Area* const parent, const float xx, const float yy) {
  evp::GUI::Window* window = new evp::GUI::Window("colorPickerWindow",parent,xx,yy,200,250,"Color Picker");
  float x,y,dx,dy;
  window->childSize(dx,dy);
  window->childOffset(x,y);
  
  float* cols = new float[3]();
  cols[0] = 0;
  cols[1] = 1;
  cols[2] = 1;
  
  evp::GUI::ColorSlot* cs = new evp::GUI::ColorSlot("colorSlot",window,
		                  x,y+dy-20,dx,20,evp::Color(0,0,0));

  auto fromPicker = [cs,cols](){
    evp::Color col = evp::HSVToColor(cols[0],cols[1],cols[2]);
    //evp::Color bc = evp::ColorHue(cols[0]);
    //
    //evp::Color rc0 = bc*(1-cols[1]) + evp::Color(1,1,1)*cols[1];
    //evp::Color rc1 = rc0*(1-cols[2]) + evp::Color(0,0,0)*cols[2];
    cs->colorValIs(col);
  };

  cs->onColorIs([cols](const evp::Color c){
    evp::ColorToHSV(c, cols[0], cols[1], cols[2]);
  });

  evp::GUI::AreaDraw* dh = new evp::GUI::AreaDraw("drawHue", window,
		                                  x,y,dx,20);
  dh->onDrawIs([cols](float x, float y, float dx, float dy, float scale, sf::RenderTarget& target){
    int ni = 16;
    float ddx = dx/ni;
    for(int i=0;i<ni;i++) {
      float ii = i / (ni-1.0);
      evp::Color c  = evp::ColorHue(ii);
      evp::DrawRect(x+i*ddx, y, ddx, dy, target, c);
    }
    evp::DrawRect(x+cols[0]*dx-5,y,10,dy, target, evp::Color(0,0,0));
    evp::DrawRect(x+cols[0]*dx-3,y+3,6,dy-6, target, evp::ColorHue(cols[0]));
  });
  
  dh->onMouseDownStartIs([dh,cols,fromPicker](const bool isFirstDown, const float x, const float y){
    float v = (x-dh->globalX())/dh->dx();
    cols[0] = std::min(1.f,std::max(0.f,v));
    fromPicker();
    return isFirstDown;
  });
  dh->onMouseDownIs([dh,cols,fromPicker](const bool isCaptured, const float x, const float y, float& dx, float& dy, evp::GUI::Area* over){
    if(isCaptured) {
      float v = (x-dh->globalX())/dh->dx();
      cols[0] = std::min(1.f,std::max(0.f,v));
      fromPicker();
    }
    return true;
  });

  evp::GUI::AreaDraw* dc = new evp::GUI::AreaDraw("drawRest", window,
		                                  x,y+20,dx,dy-40);
  dc->onDrawIs([cols](float x, float y, float dx, float dy, float scale, sf::RenderTarget& target){
    int ni = 16;
    int nj = 16;
    float ddx = dx/ni;
    float ddy = dy/nj;
    //evp::Color bc = evp::ColorHue(cols[0]);
    for(int i=0;i<ni;i++) {
      for(int j=0;j<nj;j++) {
        float ii = i / (ni-1.0);
        float jj = j / (nj-1.0);
        //evp::Color cx = bc*(1-ii) + evp::Color(1,1,1)*ii;
        //evp::Color c  = cx*(1-jj) + evp::Color(0,0,0)*jj;
	evp::Color c = evp::HSVToColor(cols[0], ii, jj);
	evp::DrawRect(x+i*ddx, y+j*ddy, ddx, ddy, target, c);
      }
    }
    //evp::Color rc0 = bc*(1-cols[1]) + evp::Color(1,1,1)*cols[1];
    //evp::Color rc1 = rc0*(1-cols[2]) + evp::Color(0,0,0)*cols[2];
    evp::Color c = evp::HSVToColor(cols[0], cols[1], cols[2]);
    evp::DrawOval(x+cols[1]*dx-5, y+cols[2]*dy-5, 10,10, target, evp::Color(1,1,1));
    evp::DrawOval(x+cols[1]*dx-4, y+cols[2]*dy-4, 8,8, target, evp::Color(0,0,0));
    evp::DrawOval(x+cols[1]*dx-3, y+cols[2]*dy-3, 6,6, target, c);
  });
  dc->onMouseDownStartIs([dc,cols,fromPicker](const bool isFirstDown, const float x, const float y){
    float vx = (x-dc->globalX())/dc->dx();
    float vy = (y-dc->globalY())/dc->dy();
    cols[1] = std::min(1.f,std::max(0.f,vx));
    cols[2] = std::min(1.f,std::max(0.f,vy));
    fromPicker();
    return isFirstDown;
  });
  dc->onMouseDownIs([dc,cols,fromPicker](const bool isCaptured, const float x, const float y, float& dx, float& dy, evp::GUI::Area* over){
    if(isCaptured) {
      float vx = (x-dc->globalX())/dc->dx();
      float vy = (y-dc->globalY())/dc->dy();
      cols[1] = std::min(1.f,std::max(0.f,vx));
      cols[2] = std::min(1.f,std::max(0.f,vy));
      fromPicker();
    }
    return true;
  });

}
