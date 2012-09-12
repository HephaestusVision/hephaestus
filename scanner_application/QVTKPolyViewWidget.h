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
#ifndef __QVTKPolyViewWidget_h__
#define __QVTKPolyViewWidget_h__

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
#define setInputData(x,y) ((x)->SetInput(y))
#else
#define setInputData(x,y) ((x)->SetInputData(y))
#endif

#include <QVTKWidget.h>

class QVTKPolyViewWidget : public QVTKWidget {
public:
  QVTKPolyViewWidget(vtkPolyData * src);
  virtual ~QVTKPolyViewWidget();
  void newSource(vtkPolyData * newPolySource);
private:
  QVTKPolyViewWidget(const QVTKPolyViewWidget &);
  QVTKPolyViewWidget& operator=(const QVTKPolyViewWidget &);
  vtkPolyData * polySource;
  vtkPolyDataMapper * mapper;
  vtkActor * actor;
  vtkRenderWindow * renderWindow;
  vtkRenderer * renderer;
};


#endif // __QVTKPolyViewWidget_h__
