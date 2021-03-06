#include "mainwindow.h"
#include "glwidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

#include <algorithm>

MainWindow::MainWindow()
{
    m_glWidget = new GLWidget(this);

    m_servoSlider = createSlider();
    connect(m_servoSlider, &QSlider::valueChanged, 
            [this](int servoPercent) { m_glWidget->setServoRotation(GLWidget::percent2Radian(-servoPercent)); }); 

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* container = new QHBoxLayout;
    container->addWidget(m_glWidget);
    container->addWidget(m_servoSlider);

    QWidget* w = new QWidget();
    w->setLayout(container);
    mainLayout->addWidget(w);
    m_stateLabel = new QLabel(tr("Positions"), this);
    mainLayout->addWidget(m_stateLabel);

    setLayout(mainLayout);

    m_servoSlider->setValue(0);

    setWindowTitle(tr("Wing Servo Kinematics"));
}

[[nodiscard]] QSlider* MainWindow::createSlider() const noexcept {
  QSlider* slider = nullptr;
  if (slider = new (std::nothrow) QSlider(Qt::Vertical)) {
    slider->setRange(-150, 150);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksRight);
  }
  return slider;
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Space) {
    m_servoSlider->setValue(0);
  } else {
    QWidget::keyPressEvent(e);
  }
}

void MainWindow::setStateText(const QString& txt) {
  m_stateLabel->setText(txt);
}

void MainWindow::setSliderDegree(RadianValue angle) {
  const int sliderValue = std::clamp<int>(-GLWidget::radian2Percent(angle), m_servoSlider->minimum(), m_servoSlider->maximum());
  m_servoSlider->setValue(sliderValue);
  update();
}

void MainWindow::wheelEvent(QWheelEvent* e) {
  QPoint n = e->angleDelta();
  const int dx = n.x();
  const int dy = n.y();
  const int d = (abs(dx) > abs(dy)) ? dx : dy;

  const int sliderValue = std::clamp<int>(m_servoSlider->value()+std::round(0.02*d), m_servoSlider->minimum(), m_servoSlider->maximum());
  m_servoSlider->setValue(sliderValue);
  update();
}
