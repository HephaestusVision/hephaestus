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

// VTK includes 
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
#define setInputData(x,y) ((x)->SetInput(y))
#else
#define setInputData(x,y) ((x)->SetInputData(y))
#endif

#include <cstdlib> //rand()

#include <QApplication>
#include "PolyViewMainWindow.h"
#include "QVTKPolyViewWidget.h"


/**
   returns a new VTKPolyData surface with random colors  
*/
vtkPolyData * source()
{
  // std::srand ( std::time(NULL) );
  vtkPolyData * data = vtkPolyData::New();
   if ((rand() % 2) == 0)
    {
    vtkSmartPointer<vtkSphereSource> src = 
      vtkSmartPointer<vtkSphereSource>::New();
    src->SetThetaResolution(32);
    src->SetPhiResolution(32);
    src->Update();
    data->ShallowCopy(src->GetOutput());
    }
  else
    {
    vtkSmartPointer<vtkCubeSource> src2 = 
      vtkSmartPointer<vtkCubeSource>::New();
    src2->Update();
    data->ShallowCopy(src2->GetOutput());
    }    
  vtkIdType N = data->GetNumberOfPoints ();
  vtkSmartPointer<vtkUnsignedCharArray> rgb = 
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  rgb->SetNumberOfComponents(3);
  rgb->SetNumberOfTuples(N);
  for (vtkIdType i = 0; i<N ; i++)
    {
    unsigned char color [3] = {0,0,0};
    color[0] = std::rand() % 256;
    if (color[0] < 255)
      color[1] = std::rand() % (255 - color[0]);
    color[2] = 255 - color[0] - color[1];
    rgb->SetTupleValue(i,color);
    }
  rgb->SetName("RGB");
  data->GetPointData()->SetScalars(rgb);
  return data;
}

int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  PolyViewMainWindow mw(&source, &app);
  mw.show();
  return app.exec();
}
