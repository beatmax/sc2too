#pragma once

namespace ui {
  class Menu {
  public:
    void show() { active_ = true; }
    void hide() { active_ = false; }
    bool active() const { return active_; }

  private:
    bool active_{false};
  };
}
