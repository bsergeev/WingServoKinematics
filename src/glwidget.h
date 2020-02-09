#pragma once

#include "util.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector2D>

#include <cmath>

class QPainter;

class MainWindow;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit GLWidget(MainWindow* parent = nullptr);

  static constexpr double DGR_TO_RADIAN = 0.017453292519943295769236907684886;
  // 100% of servo travel equals to 40 degrees
  static constexpr RadianValue percent2Radian(double percent) noexcept { return static_cast<RadianValue>(0.4 * percent * DGR_TO_RADIAN); }
  static constexpr double radian2Percent(RadianValue radians) noexcept { return 2.5 * radians / DGR_TO_RADIAN; }
                                                                                           
  void setServoRotation(RadianValue angle);

protected:
  void initializeGL()         override;
  void resizeGL(int w, int h) override;
  void paintGL()              override;

private:
  bool calculatePositions();
  [[nodiscard]] std::pair<int, int> realToScrCoords(double x, double y) const noexcept {
    return { static_cast<int>(std::round(m_x0 + x*m_scale)),
             static_cast<int>(std::round(m_y0 - y*m_scale)) };
  }

  void drawServoArm(QPainter* painter) const;

  // Linkage geometry
#if 1
  // FLAPS
  RadianValue as0 = percent2Radian(-100.0/* % */); // radians - servo arm angle at which control is at 0
  double ls =   8.0;               // mm - servo arm length
  double bs = 0*DGR_TO_RADIAN; // radians - servo arm initial angle

  double lc =  10.0;               // mm - control surface arm length
  double bc =   -15*DGR_TO_RADIAN; // radians - control surface arm initial angle

  double xc =  58.0;               // mm - distance between servo and control surface axes

  double ac_MAXup =  7.0*DGR_TO_RADIAN; // radians - MAX control surface rotation up
  double ac_MAXdn = 70.0*DGR_TO_RADIAN; // radians - MAX control surface rotation down
#else
  // Ailerons
  RadianValue as0{};               // radians - servo arm angle at which control is at 0
  double ls =   6.0;               // mm - servo arm length
  double bs = -30.0*DGR_TO_RADIAN; // radians - servo arm initial angle

  double lc =  15.0;               // mm - control surface arm length
  double bc =  20.0*DGR_TO_RADIAN; // radians - control surface arm initial angle

  double xc =  65.0;               // mm - distance between servo and control surface axes

  double ac_MAXup =  5.0*DGR_TO_RADIAN; // radians - MAX control surface rotation up
  double ac_MAXdn = 20.0*DGR_TO_RADIAN; // radians - MAX control surface rotation down
#endif

  double L;        // mm - pushrod length (computed for as = 0)
  double x0, y0;   // servo arm's tip
  double x3, y3;   // control horn's tip

  RadianValue as{};         // radians - servo arm rotation
  RadianValue lastGoodAs{}; // radians - last known good servo arm rotation
  double ac = 0.0;          // radians - control surface rotation


  // Draw scaling params
  float m_x0 = 0.0f;
  float m_y0 = 0.0f;
  float m_scale  = 1.0f;

  MainWindow* m_parent;
};
