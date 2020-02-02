#include "glwidget.h"

#include <QMouseEvent>
#include <QPainter>

#include <math.h>

GLWidget::GLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
}

void GLWidget::setServoRotation(int servoPercent) {
  // 100% of servo travel equates to 40 degrees
  as = 0.4*servoPercent * RADIAN_INV;
  update();
}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
}

void GLWidget::calculatePositions() {
  // Initial coordinates for as = 0
  const double x0i = -ls*sin(bs);
  const double y0i = ls*cos(bs);
  const double x3i = xc - lc*sin(bc);
  const double y3i = lc*cos(bc);
  L = sqrt(pow(x3i-x0i, 2.0) + pow(y3i-y0i, 2.0));

  x0 = -ls * sin(bs + as);
  y0 =  ls * cos(bs + as);

  const double dx = xc - x0;
  const double dy = -y0;

  const double d = sqrt(dx*dx + dy*dy);

  const double a_over_d = (L*L - lc*lc)/(2.0*d*d) + 0.5;
  const double a = a_over_d * d;
  const double h = sqrt(L*L - a*a);

  x3 = x0 + a_over_d*dx + h*y0/d;
  y3 = y0 + a_over_d*dy + h*xc/d;

  ac = asin((xc - x3) / lc) - bc;

}

void GLWidget::paintGL() {
  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  QPainter painter;
  painter.begin(this);

  painter.setPen(QPen(Qt::blue));

  calculatePositions();

  auto [ix0, iy0] = realToScrCoords(0.0, 0.0);
  auto [ix1, iy1] = realToScrCoords(x0, -y0);
  painter.drawLine(ix0, iy0, ix1, iy1);

  auto [ix2, iy2] = realToScrCoords(x3, -y3);
  painter.drawLine(ix1, iy1, ix2, iy2);

  auto [ix3, iy3] = realToScrCoords(xc, 0.0);
  painter.drawLine(ix2, iy2, ix3, iy3);

  painter.setPen(QPen(Qt::black));
  auto [ix4, iy4] = realToScrCoords(xc + lc*cos(ac), -lc*sin(ac));
  painter.drawLine(ix3, iy3, ix4, iy4);

  painter.end();
}

void GLWidget::resizeGL(int w, int h) {
  if (w <= 0 || h <= 0) {
    return; // not yet initialized
  }

  const auto oldW = static_cast<float>(m_screenW),
             oldH = static_cast<float>(m_screenH);
  m_screenW = w;  m_screenH = h;

  const double minX = -ls;
  const double maxX = xc + lc;
  const double maxY = std::max(ls, lc);
  const double minY = -maxY;

  constexpr int PADDING = 10;
  const double scaleX = (m_screenW - PADDING) / (maxX - minX);
  const double scaleY = (m_screenH - PADDING) / (maxY - minY);
  m_scale = std::min(scaleX, scaleY);
  m_x0 = static_cast<decltype(m_x0)>(PADDING/2 + std::round(0.5*((m_screenW - PADDING) - (maxX + minX)*m_scale)));
  m_y0 = static_cast<decltype(m_y0)>(PADDING/2 + std::round(0.5*((m_screenH - PADDING) + (maxY + minY)*m_scale)));
}

void GLWidget::wheelEvent(QWheelEvent* e) {
  QPoint n = e->angleDelta();
  const int dx = n.x();
  const int dy = n.y();
  const int d = (abs(dx) > abs(dy)) ? dx : dy;

  const float zoom = (d > 0)? 1.2f : 1.0f/1.2f;

  QPoint screenCursorPos = e->pos();

  // ...

  update();
}

void GLWidget::mousePressEvent(QMouseEvent* e) {
  m_mousePressPosition = QVector2D(e->localPos());
  m_dragging = true;
}

void GLWidget::mouseReleaseEvent(QMouseEvent*) {
  m_dragging = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent* e) {
  if (m_dragging) {
    auto newMousePos = QVector2D(e->localPos());
    const QVector2D diff = newMousePos - m_mousePressPosition;

//     m_offsetX -= 2.0f * diff.x() / static_cast<float>(m_screenW);
//     m_offsetY += 2.0f * diff.y() / static_cast<float>(m_screenH);
     
    m_mousePressPosition = std::move(newMousePos);

    m_isZoomFit = false;

    update();
  }
}
void GLWidget::keyPressEvent(QKeyEvent* keyevent) {
  const int key = keyevent->key();
  if (key == Qt::Key_F) {
   // fitAll();
    update();
  }
}
