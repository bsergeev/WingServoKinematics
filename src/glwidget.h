#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector2D>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit GLWidget(QWidget* parent = nullptr);

public slots:
  void setServoRotation(int angle);

protected:
  void mousePressEvent  (QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseMoveEvent   (QMouseEvent* e) override;
  void wheelEvent   (QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* keyEvnt) override;

  void initializeGL()         override;
  void resizeGL(int w, int h) override;
  void paintGL()              override;

private:
  void calculatePositions();
  [[nodiscard]] std::pair<int, int> realToScrCoords(double x, double y) const noexcept {
    return { static_cast<int>(std::round(m_x0 + x*m_scale)),
             static_cast<int>(std::round(m_y0 - y*m_scale)) };
  }

  // Linkage geometry
  static constexpr double RADIAN_INV = 0.017453292519943295769236907684886;
  double ls =  10.0; // mm - servo arm length
  double bs = 0.0*RADIAN_INV; // degrees - servo arm initial angle
  double lc =  15.0; // mm - control surface arm length
  double bc =  30.0*RADIAN_INV; // degrees - control surface arm initial angle
  double xc =  75.0; // mm - distance between servo and control surface axes

  double L;        // mm - pushrod length
  double x0, y0;
  double x3, y3;

  double as = 0.0; // degrees - servo arm rotation
  double ac = 0.0; // degrees - control surface rotation



  float m_x0 = 0.0f;
  float m_y0 = 0.0f;
  float m_scale  = 1.0f;

  int  m_screenW = 0;
  int  m_screenH = 0;

  bool m_isZoomFit = true;
  bool m_dragging = false;
  QVector2D m_mousePressPosition{};
};
