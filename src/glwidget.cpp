#include "glwidget.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

#include <math.h>
#include <iostream> // <<< DEBUG

GLWidget::GLWidget(MainWindow* parent)
  : QOpenGLWidget(parent)
  , m_parent(parent)
{
  setMinimumSize(900, 300);
}

void GLWidget::setServoRotation(int degrees) {
  as = degrees * DGR_TO_RADIAN;
  update();
}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
}

bool GLWidget::calculatePositions() {
  // Initial coordinates for as = 0
  const double x0i = -ls*sin(bs);
  const double y0i = ls*cos(bs);
  const double x3i = xc - lc*sin(bc);
  const double y3i = lc*cos(bc);
  L = sqrt(pow(x3i-x0i, 2.0) + pow(y3i-y0i, 2.0));


  const double as_degr = as / DGR_TO_RADIAN;
  QString state = tr("Servo travel: %1% (%2 degr)").arg(2.5*as_degr).arg(QString::number(as_degr, 'f', 1));

  x0 = -ls * sin(bs + as);
  y0 =  ls * cos(bs + as);

  const double dx = xc - x0;
  const double dy = -y0;

  const double d = sqrt(dx*dx + dy*dy);
  if (d > L + lc) {
    as = lastGoodAs;
    m_parent->setSliderDegree(as / DGR_TO_RADIAN);
    m_parent->setStateText(state + tr(" - Linkage bind"));
    return false;
  }

  const double a_over_d = (L*L - lc*lc)/(2.0*d*d) + 0.5;
  const double a = a_over_d * d;
  const double h = sqrt(L*L - a*a);

  x3 = x0 + a_over_d*dx + h*y0/d;
  y3 = y0 + a_over_d*dy + h*xc/d;

  ac = asin((xc - x3) / lc) - bc;
  state += tr(", control surface deflection: %1 degr").arg(QString::number(-ac / DGR_TO_RADIAN, 'f', 2));

  const bool upExceeded = (ac < 0.0)? -ac > ac_MAXup : false;
  const bool dnExceeded = (ac > 0.0)?  ac > ac_MAXdn : false;
  if (upExceeded || dnExceeded) {
    as = lastGoodAs;
    m_parent->setSliderDegree(as / DGR_TO_RADIAN);
    state += tr(" - exceeded deflection %1").arg((upExceeded)? tr("UP") : tr("DOWN"));
  } else {
    lastGoodAs = as;
  }
  m_parent->setStateText(state);
  return (!upExceeded && !dnExceeded);
}

void GLWidget::drawServoArm(QPainter* painter) const {
  const double rs1 = 2.5, rs2 = 1.25;
  const double r_bolt = 0.75;

  constexpr double PI_2 = 1.570796326794896619231322;
  const size_t Narc = 10;

  std::vector<QPoint> points;
  points.reserve(2 * Narc + 2);

  const double b = asin((rs1-rs2)/ls);
  const double as1 = as + PI_2 - b;
  const double da1 = 2.0*(PI_2 + b);
  for (size_t i = 0; i <= Narc; ++i) {
    const double a = PI_2 + bs + as1 + i*da1/Narc;
    const double x =  rs1 * cos(a);
    const double y = -rs1 * sin(a);
    const auto [ix, iy] = realToScrCoords(x, y);
    points.emplace_back(ix, iy);
  }

  const double ae2 = as + PI_2 - b;
  const double da2 = 2.0*(PI_2 - b);
  for (size_t i = 0; i <= Narc; ++i) {
    const double a = PI_2 + bs + ae2 - da2 + i*da2/Narc;
    const double x =   x0 + rs2 * cos(a);
    const double y = -(y0 + rs2 * sin(a));
    const auto [ix, iy] = realToScrCoords(x, y);
    points.emplace_back(ix, iy);
  }

  painter->setPen(QPen(Qt::black));
  painter->setBrush(QBrush(Qt::white));
  painter->drawPolygon(points.data(), static_cast<int>(points.size()));

  const auto [ix, iy] = realToScrCoords(0, 0);
  const auto [ix2, iy2] = realToScrCoords(r_bolt, 0);
  const int ir_bolr = ix2 - ix;
  painter->setBrush(QBrush(Qt::black));
  painter->drawEllipse(QPoint(ix, iy), ir_bolr, ir_bolr);
}

void GLWidget::paintGL() {
  if (calculatePositions()) {
    glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter;
    painter.begin(this);

    drawServoArm(&painter);
    painter.setPen(QPen(Qt::blue, 2));

    auto [ix1, iy1] = realToScrCoords(x0, -y0);
    auto [ix2, iy2] = realToScrCoords(x3, -y3);
    painter.drawLine(ix1, iy1, ix2, iy2);

    auto [ix3, iy3] = realToScrCoords(xc, 0.0);
    painter.drawLine(ix2, iy2, ix3, iy3);

    painter.setPen(QPen(Qt::black, 1));
    auto [ix4, iy4] = realToScrCoords(xc + lc * cos(ac), -lc * sin(ac));
    painter.drawLine(ix3, iy3, ix4, iy4);

    painter.end();
  }
}

void GLWidget::resizeGL(int w, int h) {
  if (w <= 0 || h <= 0) {
    return; // not yet initialized
  }

  const double minX = -ls;
  const double maxX = xc + lc;
  const double maxY = std::max(ls, lc);
  const double minY = -maxY;

  constexpr int PADDING = 10;
  const double scaleX = (w - PADDING) / (maxX - minX);
  const double scaleY = (h - PADDING) / (maxY - minY);
  m_scale = std::min(scaleX, scaleY);
  m_x0 = static_cast<decltype(m_x0)>(PADDING/2 + std::round(0.5*((w - PADDING) - (maxX + minX)*m_scale)));
  m_y0 = static_cast<decltype(m_y0)>(PADDING/2 + std::round(0.5*((h - PADDING) + (maxY + minY)*m_scale)));
}
