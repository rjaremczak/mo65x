#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

class RenderWindow
{
 public:
  RenderWindow(unsigned width, unsigned height, const std::string& title);
  ~RenderWindow();

  bool isOpen() const;
  void update();
  void renderAndDisplay();
  void setTitle(const std::string&);
  void setBackgroundColor(float[3]);

 private:
  sf::RenderWindow m_window;
  sf::Clock m_clock;
  sf::Color m_backgroundColor;

  void processEvents();
};
