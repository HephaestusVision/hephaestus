/**
	@file CameraWidget.h

	@authors
  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/
		
  @copyright
	Copyright 2012 University of North Carolina at Chapel Hill.

  @copyright
  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License at
    LICENSE.txt in this repository or
    http://www.apache.org/licenses/LICENSE-2.0

  @copyright
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

/**
  The CameraWidget class can be used where a QWidget can be used.  It
  will talk to a camera using the open-cv and open-kinect libraries.

	Superclass : http://qt-project.org/doc/qt-4.8/QGLWidget.html

  To find out whether the camera is in working order, check the
  isGood() method.
*/
class CameraWidget : public QGLWidget {
  Q_OBJECT // include this if you use Qt signals/slots
public:
	/**
		 Constructor
		 @param parent is passed to the QWidget constructor.
		 @param cameraIndex is the freenect_sync_get_rgb_cv() function;
 */
  CameraWidget(QWidget * parent = NULL, int cameraIndex=0);
  ~CameraWidget();
	/**
		 @returns does the freenect_sync_get_rgb_cv() function return a non-null value?
	*/
  bool isGood();
protected:
	/** used by the QGLWidget class */
  void initializeGL();
	/** used by the QGLWidget class */
  void resizeGL(int w, int h);
  /** used by the QGLWidget class */
  void paintGL();
  /** used by the QGLWidget class */
  void mousePressEvent(QMouseEvent *event);
  /** used by the QGLWidget class */
  void mouseMoveEvent(QMouseEvent *event);
  /** used by the QGLWidget class */
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
