#ifndef UDEN_UI
#define UDEN_UI


#include <raylib.h>
#include <string>
#include <vector>
#include <functional>

class Ui {
public:
  virtual void move(Vector2 distance)=0;
  virtual void setPos(Vector2 pos)=0;
  virtual void draw()=0;
  virtual void receiveMousePos(Vector2 mousePos)=0;

  void setOnClick(std::function<void(Ui*)>);
  void setOnMouseEnter(std::function<void(Ui*)>);
  void setOnMouseExit(std::function<void(Ui*)>);

protected:
  std::function<void(Ui*)> onClick;
  std::function<void(Ui*)> onMouseEnter;
  std::function<void(Ui*)> onMouseExit;
};

class UiText: public Ui {
public:
  UiText();
  UiText(std::string text);
  ~UiText();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setColor(Color c);
  void setText(std::string text);

private:
  bool hovered = false;

  Vector2 pos;
  Vector2 size;

  std::string text;
  Color color;
};

class UiDropDown: public Ui {
public:
  UiDropDown(std::string label, std::vector<std::string> options);
  ~UiDropDown();

  void move(Vector2 distance);
  void setPos(Vector2 pos);
  void draw();
  void receiveMousePos(Vector2 mousePos);

  void setOnSelected(std::function<void(std::string)>);

private:
  Vector2 pos;
  Vector2 btnSize;
  Vector2 listSize;

  std::vector<std::string> options;
  Color bgColor;
  Color fgColor;

  UiText label;
  bool hovered = false;
  bool open = false;
};

#endif
