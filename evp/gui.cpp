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


