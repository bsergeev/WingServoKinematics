#include "mainwindow.h"
#include "glwidget.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QApplication>

MainWindow::MainWindow()
{
    glWidget = new GLWidget(this);

    servoSlider = createSlider();
    connect(servoSlider, &QSlider::valueChanged, glWidget, &GLWidget::setServoRotation);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* container = new QHBoxLayout;
    container->addWidget(glWidget);
    container->addWidget(servoSlider);

    QWidget* w = new QWidget();
    w->setLayout(container);
    mainLayout->addWidget(w);
//     dockBtn = new QPushButton(tr("Undock"), this);
//     connect(dockBtn, &QPushButton::clicked, this, &MainWindow::dockUndock);
//     mainLayout->addWidget(dockBtn);

    setLayout(mainLayout);

    servoSlider->setValue(0);

    setWindowTitle(tr("Wing Servo Kinematics"));
}

[[nodiscard]] QSlider* MainWindow::createSlider() const noexcept {
  QSlider* slider = nullptr;
  if (slider = new (std::nothrow) QSlider(Qt::Vertical)) {
    slider->setRange(-150, 150);
    slider->setSingleStep(5);
    slider->setPageStep(30);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksRight);
  }
  return slider;
}
