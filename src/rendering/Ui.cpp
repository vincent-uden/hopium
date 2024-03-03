#include "Ui.h"
#include "raylib.h"
#include <cmath>
#include <format>

namespace Ui {

std::shared_ptr<Colorscheme> Ui::colorscheme = nullptr;

void Ui::setOnClick(std::function<void (Ui *)> f) {
  onClick = f;
}

void Ui::setOnMouseEnter(std::function<void (Ui *)> f) {
  onMouseEnter = f;
}

void Ui::setOnMouseExit(std::function<void (Ui *)> f) {
  onMouseExit = f;
}

/* End of namespace */ }
