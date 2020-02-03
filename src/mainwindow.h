#pragma once

#include <QString>
#include <QWidget>

class QKeyEvent;
class QLabel;
class QSlider;
class QWheelEvent;

class GLWidget;

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow();

  void setStateText(const QString& txt);
  void setSliderDegree(int value);

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void wheelEvent (QWheelEvent* event) override;

private:
  [[nodiscard]] QSlider* createSlider() const noexcept;

  GLWidget* m_glWidget     = nullptr;
  QSlider*  m_servoSlider  = nullptr;
  QLabel*   m_stateLabel   = nullptr;
};
