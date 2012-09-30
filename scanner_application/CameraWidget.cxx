/*
  DepthScannerMainWindow.h

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

// a lot of source copied from 
//   http://rafaelpalomar.net/blog/2010/sep/25/displaying-opencv-webcam-output-using-qt-and-opengl

#include "CameraWidget.h"

#include <iostream>
#include <cassert>

#include <QTimer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QKeyEvent>

#include "cv.h" // open-cv
#include "libfreenect_cv.h" // open-kinect

CameraWidget::CameraWidget(QWidget * parent) : QGLWidget(parent)
{
  //Initialize variable members
  this->timer = new QTimer();
  this->image = freenect_sync_get_rgb_cv(0);
  //Connect the timer signal with the capture action
  connect(this->timer, SIGNAL(timeout()), this, SLOT(captureFrame()));
  //Start the timer scheduled for firing every 33ms (30fps)
  if (this->image != NULL)
    this->timer->start(33);
  else
    std::cerr 
      << "freenect_sync_get_rgb_cv returns NULL.\n"
      << "Probably unable to connect to the Kinect.  "
      << "Is it plugged in?\n";
  //FIXME: better error handling.
 }

CameraWidget::~CameraWidget()
{
  delete this->timer;
  // FIXME: determine if cleanup needs to be perfomred, maybe with
  // valgrind.
}
void CameraWidget::initializeGL()
{
  //Adjust the viewport
  glViewport(0,0,this->width(), this->height());
  
  //Adjust the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-this->width()/2,
          this->width()/2, this->height()/2, -this->height()/2, -1, 1); 
}

void CameraWidget::resizeGL(int w, int h) { }

void CameraWidget::paintGL()
{
  //Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  //Set the raster position
  /*
    The position seems to be the inverse because the rendering is
    affected by the glPixelZoom call.
  */
  glRasterPos2i(this->width()/2,-this->height()/2);
  //Inver the image (the data coming from OpenCV is inverted)
  glPixelZoom(-1.0f,-1.0f);

  //Draw image from OpenCV capture
  if (this->image != NULL)
    glDrawPixels(
      this->image->width,
      this->image->height, GL_RGB, GL_UNSIGNED_BYTE,
      this->image->imageData);
}
void CameraWidget::mousePressEvent(QMouseEvent *event) { }
void CameraWidget::mouseMoveEvent(QMouseEvent *event) { }
void CameraWidget::keyPressEvent(QKeyEvent *event) { }

void CameraWidget::captureFrame()
{ 
  this->image = freenect_sync_get_rgb_cv(0);
  glDraw(); 
}
