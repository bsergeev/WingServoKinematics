#pragma once

#include <QWidget>

class QSlider;

class GLWidget;

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow();

private:
  [[nodiscard]] QSlider* createSlider() const noexcept;

  GLWidget* glWidget     = nullptr;
  QSlider*  servoSlider  = nullptr;
};
