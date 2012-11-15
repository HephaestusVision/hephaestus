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
#ifndef DEPTHSCANNERMAINWINDOW_H
#define DEPTHSCANNERMAINWINDOW_H

#include <QMainWindow>
#include "QVTKPolyViewWidget.h"
#include "LoginDialog.h"
class QApplication;
class Cloudy;
class CameraWidget;
class vtkPolyData;

class DepthScannerMainWindow : public QMainWindow {
  Q_OBJECT
public:
  DepthScannerMainWindow();
  ~DepthScannerMainWindow();
public slots:
  // function executed when  you click Create
  void create();
  // function executed when you click Update
  void update();
  // function executed when  you click Save
  void save();
  // function executed when  you click Load
  void load();
  // function executed when  you click Upload
  void upload();
public:
  vtkPolyData * pointCloud;
  QVTKPolyViewWidget qVTKPolyViewWidget ;
  Cloudy * cloudy;
  CameraWidget * camera;

  // The LoginDialog is hidden when it completes, but should keep the
  // data fields populated.
  LoginDialog loginDialog;
private:
  DepthScannerMainWindow(const DepthScannerMainWindow &);
  DepthScannerMainWindow& operator=(const DepthScannerMainWindow &);
};
#endif /* DEPTHSCANNERMAINWINDOW_H */
