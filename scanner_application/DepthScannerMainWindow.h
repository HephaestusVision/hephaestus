/**
	@file DepthScannerMainWindow.h

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

#ifndef DEPTHSCANNERMAINWINDOW_H
#define DEPTHSCANNERMAINWINDOW_H

#include <QMainWindow>
#include "QVTKPolyViewWidget.h"
#include "LoginDialog.h"
#include "Parameters.h"
class QApplication;
class Cloudy;
class CameraWidget;
class vtkPolyData;

/**
	 subclass of <a
	 href="http://doc.qt.digia.com/qt/qmainwindow.html">QMainWindow</a>.
	 Has a window with camera view, another window with the rendered
	 object (via VTK), and a toolbar with command buttons.  */
class DepthScannerMainWindow : public QMainWindow {
  Q_OBJECT
public:
	/** constructor */
  DepthScannerMainWindow();
  ~DepthScannerMainWindow();
public slots:
  /**
    function executed when  you click Create.  Calls Cloudy::CreatePointCloud() */
  void create();
  /**
    function executed when you click Update.  Calls Cloudy::UpdatePointCloud() */
  void update();
  /**
    function executed when  you click Save. Calls QVTKPolyViewWidget::saveVTK() */
  void save();
  /**
    function executed when  you click Load. Calls QVTKPolyViewWidget::openVTK() */
  void load();
  /**
    function executed when  you click Upload.  Calls upload_to_midas() */
  void upload();
  /**
    function executed when  you click Settings.   */
  void settings();
public:
	Parameters parameters;
  vtkPolyData * pointCloud;
  QVTKPolyViewWidget qVTKPolyViewWidget ;
  Cloudy * cloudy;
  CameraWidget * camera;

  /**
    The LoginDialog is hidden when it completes, but should keep the
	data fields populated. */
  LoginDialog loginDialog;
private:
  DepthScannerMainWindow(const DepthScannerMainWindow &);
  DepthScannerMainWindow& operator=(const DepthScannerMainWindow &);
};
#endif /* DEPTHSCANNERMAINWINDOW_H */
