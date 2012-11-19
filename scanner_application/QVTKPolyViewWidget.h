/**
  @file QVTKPolyViewWidget.h

	VTK-QT interface class

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
#ifndef QVTKPOLYVIEWWIDGET_H
#define QVTKPOLYVIEWWIDGET_H

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkRenderWindow;
class vtkRenderer;

#include <QVTKWidget.h>
#include <QString>

/**
	 Subclass of QVTKWidget capable of displaying a single vtkPolyData object.
	 The rendered object can be updated by calling the newSource() method.

	 The source must not be deleted since this class only stores a
	 pointer to the source.
 */
class QVTKPolyViewWidget : public QVTKWidget {
public:
	/**
		 Constructor
		 @param parent Passed to superclass,
		 @param src source dataset pointer,
	 */
  QVTKPolyViewWidget(QWidget *parent=NULL, vtkPolyData * src = NULL);
  virtual ~QVTKPolyViewWidget();
	/**
		 swap out the source and re-render
	 */
  void newSource(vtkPolyData * newPolySource);

	/**
		 Opens a VTK or VTP PolyData file and copies the contents into the
		 vtkPolyData object pointed to.
	 */
  static void openVTK(QString filename, vtkPolyData * polyData);

	/**
		 Saves a vtkPolyData to a file with the given filename.
	 */
  static void saveVTK(QString filename, vtkPolyData * polyData);

	/**
		 Modifies the camera. */
  void SetCamera(const double FocalPoint[3], const double position[3], const double viewUp[3], double viewAngle);

	/**
		 Find out the current camera settings. */
  void GetCamera(double FocalPoint[3], double position[3], double viewUp[3], double * viewAngle);
	/**
		 reset (center) the camera. */
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
