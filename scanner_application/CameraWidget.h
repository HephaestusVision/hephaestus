#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtOpenGL/QGLWidget>

class CameraWidget : public QGLWidget {
  Q_OBJECT // include this if you use Qt signals/slots
public:
  CameraWidget(QWidget * parent = NULL);
  ~CameraWidget();
protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);
private:
  CameraWidget(const CameraWidget &);  // noncopyable resource
  CameraWidget& operator=(const CameraWidget &); // noncopyable resource
};

#endif /* CAMERAWIDGET_H */
