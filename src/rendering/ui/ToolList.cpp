#include "ToolList.h"

namespace Ui {

ToolList::ToolList() {
  btnNames.push_back("Sketch");
  btnNames.push_back("Point");
  btnNames.push_back("Line");
  btnNames.push_back("Extrude");
  pos = { 0, 0 };

  btnSize = {80, 80};
  margin = 4;

  int i = 0;
  for (std::string& btnName : btnNames) {
    std::shared_ptr<Text> btnLbl(new Text(btnName));
    Vector2 textSize = MeasureTextEx(colorscheme->font, btnName.c_str(), 20, 1);
    btnLbl->setPos({(btnSize.x + margin) * i + (btnSize.x - textSize.x) / 2.0f, (btnSize.y - textSize.y) / 2.0f});
    btnLbl->setColor({255, 255, 255, 255});
    btnLbls.push_back(btnLbl);

    std::shared_ptr<Rect> btnBg(new Rect());
    btnBg->setSize(btnSize);
    btnBg->setColor(colorscheme->secondary);
    btnBg->setPos({(btnSize.x + margin) * i, 0});
    btnBg->setOnMouseEnter([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor(colorscheme->secondaryVariant);
    });
    btnBg->setOnMouseExit([](Ui* p) {
        Rect* bg = static_cast<Rect*>(p);
        bg->setColor(colorscheme->secondary);
    });
    btnBgs.push_back(btnBg);

    i++;
  }

  btnBgs[0]->setOnClick([](Ui* p) {
      EventQueue::getInstance()->postEvent(toggleSketchMode {});
  });
  btnBgs[1]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(togglePointMode {});
      }
  });
  btnBgs[2]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(toggleLineMode {});
      }
  });
  btnBgs[3]->setOnClick([](Ui* p) {
      if (ApplicationState::getInstance()->sketchModeActive) {
        EventQueue::getInstance()->postEvent(toggleExtrudeMode {});
      }
  });
}

ToolList::~ToolList() {
}

void ToolList::move(Vector2 distance) {
  for (auto& lbl : btnLbls) {
    lbl->move(distance);
  }
  for (auto& bg : btnBgs) {
    bg->move(distance);
  }
}

void ToolList::setPos(Vector2 pos) {
    Vector2 diff;
    diff.x = pos.x - this->pos.x;
    diff.y = pos.y - this->pos.y;
    move(diff);
}

void ToolList::draw() {
  int i = 0;
  for (auto& bg : btnBgs) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      bg->draw();
    }
    ++i;
  }
  i = 0;
  for (auto& lbl : btnLbls) {
    if (i == 0) {
      lbl->draw();
    } else {
      ApplicationState* state = ApplicationState::getInstance();
      switch (i) {
      case 1:
        if (state->modeStack.isActive(state->point)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      case 2:
        if (state->modeStack.isActive(state->line)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      case 3:
        if (state->modeStack.isActive(state->extrude)) {
          lbl->setColor(colorscheme->active);
        } else {
          lbl->setColor(colorscheme->onBackground);
        }
        break;
      };
      if (ApplicationState::getInstance()->sketchModeActive) {
        lbl->draw();
      }
    }
    ++i;
  }
}

void ToolList::receiveMousePos(Vector2 mousePos) {
  int i = 0;
  for (auto& bg : btnBgs) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      bg->receiveMousePos(mousePos);
    }
    ++i;
  }
  i = 0;
  for (auto& lbl : btnLbls) {
    if (i == 0 || ApplicationState::getInstance()->sketchModeActive) {
      lbl->receiveMousePos(mousePos);
    }
    ++i;
  }
}

void ToolList::receiveMouseDown(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseDown(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseDown(mousePos);
  }
}

void ToolList::receiveMouseUp(Vector2 mousePos) {
  for (auto& bg : btnBgs) {
    bg->receiveMouseUp(mousePos);
  }
  for (auto& lbl : btnLbls) {
    lbl->receiveMouseUp(mousePos);
  }
}

void ToolList::receiveMouseWheel(Vector2 mousePos, float movement) {
}

Vector2 ToolList::getSize() {
  return Vector2 { (btnSize.x + margin) * btnNames.size() - margin, btnSize.y };
}

}
