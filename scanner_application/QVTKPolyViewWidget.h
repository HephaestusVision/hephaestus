/*
  An example of a VTK-QT application.

  Copyright 2012 University of North Carolina at Chapel Hill.

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
#ifndef QVTKPOLYVIEWWIDGET_H
#define QVTKPOLYVIEWWIDGET_H

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkRenderWindow;
class vtkRenderer;

#include <QVTKWidget.h>
#include <QString>

class QVTKPolyViewWidget : public QVTKWidget {
public:
  QVTKPolyViewWidget(vtkPolyData * src);
  virtual ~QVTKPolyViewWidget();
  void newSource(vtkPolyData * newPolySource);
  static void openVTK(QString filename, vtkPolyData * polyData);
  static void saveVTK(QString filename, vtkPolyData * polyData);
  void SetCamera(const double FocalPoint[3], const double position[3], const double viewUp[3], double viewAngle);
  void GetCamera(double FocalPoint[3], double position[3], double viewUp[3], double * viewAngle);
	void ResetCamera();

private:
  QVTKPolyViewWidget(const QVTKPolyViewWidget &);
  QVTKPolyViewWidget& operator=(const QVTKPolyViewWidget &);
  vtkPolyData * polySource;
  vtkPolyDataMapper * mapper;
  vtkActor * actor;
  vtkRenderWindow * renderWindow;
  vtkRenderer * renderer;
};

#endif /* QVTKPOLYVIEWWIDGET_H */
