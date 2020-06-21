#include "renderwindow.h"

RenderWindow::RenderWindow(unsigned width, unsigned height, const std::string& title)
    : m_window(sf::VideoMode(width, height), title) {
  m_window.setVerticalSyncEnabled(true);
  ImGui::SFML::Init(m_window);
  m_window.resetGLStates();
}

RenderWindow::~RenderWindow() {
  ImGui::SFML::Shutdown();
}

bool RenderWindow::isOpen() const {
  return m_window.isOpen();
}

void RenderWindow::setTitle(const std::string& title) {
  m_window.setTitle(title);
}

void RenderWindow::setBackgroundColor(float color[3]) {
  m_backgroundColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
  m_backgroundColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
  m_backgroundColor.b = static_cast<sf::Uint8>(color[2] * 255.f);
}

void RenderWindow::update() {
  processEvents();
  ImGui::SFML::Update(m_window, m_clock.restart());
}

void RenderWindow::renderAndDisplay() {
  m_window.clear(m_backgroundColor);
  ImGui::SFML::Render(m_window);
  m_window.display();
}

void RenderWindow::processEvents() {
  sf::Event event;
  while (m_window.pollEvent(event)) {
    ImGui::SFML::ProcessEvent(event);

    if (event.type == sf::Event::Closed) { m_window.close(); }
  }
}
