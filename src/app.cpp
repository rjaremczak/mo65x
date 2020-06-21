#include "app.h"
#include "commonutils.h"

static const std::string ProjectName = "mo65x";
static const std::string ConfigFileName = "config.json";
static const std::string ProjectVersion = "1.01";

App::App() {}

int App::run() {
  initConfigStorage();
  initRenderWindow();

  sf::Color bgColor;

  float color[3] = { 0.f, 0.f, 0.f };

  // let's use char array as buffer, see next part
  // for instructions on using std::string with ImGui
  char windowTitle[255] = "ImGui + SFML = <3";

  bool demoWindow = false;

  while (m_renderWindow->isOpen()) {
    m_renderWindow->update();


    ImGui::Begin("Sample window"); // begin window

    // Background color edit
    if (ImGui::ColorEdit3("Background color", color)) {
      m_renderWindow->setBackgroundColor(color);
    }

    // Window title text edit
    ImGui::InputText("Window title", windowTitle, sizeof(windowTitle));

    if (ImGui::Button("Update window title")) {
      m_renderWindow->setTitle(windowTitle);
    }

    ImGui::Checkbox("Show demo window", &demoWindow);

    if(demoWindow) ImGui::ShowDemoWindow();

    ImGui::End(); // end window

    m_renderWindow->renderAndDisplay();
  }

  return 0;
}

void App::errorMessage(const std::string &)
{

}

void App::initConfigStorage() {
  const auto appDir = mindpart::home_directory() / (std::string(".") + ProjectName);
  if (!std::filesystem::exists(appDir)) std::filesystem::create_directory(appDir);
  m_configStorage = std::make_unique<FileDataStorage<Config>>(appDir / ConfigFileName);
  m_config = m_configStorage->readOrInit();
}

void App::initRenderWindow() {
  m_renderWindow = std::make_unique<RenderWindow>(800,600,ProjectName);
}

void App::loadMemoryFromFile(Address start, const std::filesystem::path& fpath) {
  std::ifstream is(fpath, std::ios::in | std::ios::binary);
  if(is.is_open()) {
    const auto size = std::filesystem::file_size(fpath);
    Data buf(static_cast<size_t>(size));
    if(is.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(size))) {
      m_emulator.writeMemory(start, buf);
      return;
    }
  }
  errorMessage("load error");
}

void App::saveMemoryToFile(AddressRange range, const std::filesystem::path& fpath) {
  std::ofstream os(fpath, std::ios::out | std::ios::binary);
  if(os.is_open()) {
    Data buf(range.size());
    m_emulator.readMemory(range, buf);
    if(os.write(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()))) return;
  }
  errorMessage("save error");
}
