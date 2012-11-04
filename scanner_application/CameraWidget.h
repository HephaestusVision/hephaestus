/*
  CameraWidget.h

  Copyright 2012 University of North Carolina at Chapel Hill.

  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/

  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License at

    LICENSE.txt in this repository or
    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.
*/
#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QtOpenGL/QGLWidget>

class QTimer;
class QMouseEvent;
class QKeyEvent;
struct CvCapture;
struct _IplImage;

class CameraWidget : public QGLWidget {
  Q_OBJECT // include this if you use Qt signals/slots
public:
  CameraWidget(QWidget * parent = NULL, int cameraIndex=0);
  ~CameraWidget();
  bool isGood();
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

private:
	int cameraIndex;
  bool m_isGood;
  QTimer * timer;
  CvCapture * capture;
  _IplImage * image;
private slots:
  void captureFrame();
};

#endif /* CAMERAWIDGET_H */
