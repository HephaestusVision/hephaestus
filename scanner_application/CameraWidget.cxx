#include "CameraWidget.h"

CameraWidget::CameraWidget(QWidget * parent) : QGLWidget(parent) { }
CameraWidget::~CameraWidget() { }
void CameraWidget::initializeGL() { }
void CameraWidget::resizeGL(int w, int h) { }
void CameraWidget::paintGL() { }
void CameraWidget::mousePressEvent(QMouseEvent *event) { }
void CameraWidget::mouseMoveEvent(QMouseEvent *event) { }
void CameraWidget::keyPressEvent(QKeyEvent *event) { }
