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
#ifndef __PolyViewMainWindow_h__
#define __PolyViewMainWindow_h__

#include <QMainWindow>
#include "QVTKPolyViewWidget.h"
class vtkPolyData;
typedef vtkPolyData * (*PolyDataSourceFunction)();

class PolyViewMainWindow : public QMainWindow {
  Q_OBJECT
public:
  PolyViewMainWindow(PolyDataSourceFunction srcFn, QApplication * app);
  ~PolyViewMainWindow();
public slots:
  void replaceSrc();
private:
  PolyDataSourceFunction srcFn;
  vtkPolyData * currentSource;
  QVTKPolyViewWidget polyWidget; 
  PolyViewMainWindow(const PolyViewMainWindow &);
  PolyViewMainWindow& operator=(const PolyViewMainWindow &);
};

#endif
