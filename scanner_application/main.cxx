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

#include <vtkSmartPointer.h>
#include <vtkActor.h>
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

#include <cstdlib> //Rand()

#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <QToolBar>
#include <QAction>
#include <QVTKWidget.h>


vtkPolyData * source()
{
  vtkSmartPointer<vtkSphereSource> src = 
    vtkSmartPointer<vtkSphereSource>::New();
  src->SetThetaResolution(32);
  src->SetPhiResolution(32);
  vtkPolyData * data = vtkPolyData::New();
  src->Update();
  data->ShallowCopy(src->GetOutput());
  vtkIdType N = data->GetNumberOfPoints ();
  vtkSmartPointer<vtkUnsignedCharArray> rgb = 
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  rgb->SetNumberOfComponents(3);
  rgb->SetNumberOfTuples(N);
  for (vtkIdType i = 0; i<N ; i++)
    {
    unsigned char color [3] = {0,0,0};
    color[0] = rand() % 256;
    if (color[0] < 255)
      color[1] = rand() % (255 - color[0]);
    color[2] = 255 - color[0] - color[1];
    rgb->SetTupleValue(i,color);
    }
  rgb->SetName("RGB");
  data->GetPointData()->SetScalars(rgb);
  return data;
}

class QVTKPolyViewWidget : public QVTKWidget {
public:
  QVTKPolyViewWidget(vtkPolyData * src)
  {
    vtkSmartPointer<vtkPolyDataMapper> mapper = 
      vtkSmartPointer<vtkPolyDataMapper>::New();
    setInputData(mapper,src);
    vtkSmartPointer<vtkActor> actor = 
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    vtkSmartPointer<vtkRenderWindow> renderWindow = 
      vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> renderer = 
      vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    renderer->AddActor(actor);
    renderer->ResetCamera();
    this->resize(480, 480);
    this->setMinimumSize(480, 480);
    this->SetRenderWindow(renderWindow);
    this->show();
  }
};

class PolyViewMainWindow : public QMainWindow {
public:
  PolyViewMainWindow(vtkPolyData * src, QApplication * app):
    polyWidget(src)
  {
    this->setCentralWidget(&(this->polyWidget));
    QAction * exit_action = new QAction(QString("E&xit"), this);
    QToolBar * tool = this->addToolBar(QString("Tools"));
    exit_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    exit_action->setStatusTip("Exit application. Ctrl-q");
    exit_action->setToolTip("Exit application. Ctrl-q");
    this->connect(exit_action, SIGNAL(triggered()),app,SLOT(quit()));
    tool->addAction(exit_action);
    this->setWindowTitle("Hello, QT-VTK!");
    this->adjustSize();
    this->show();
  }
private:
  QVTKPolyViewWidget polyWidget; 
};

int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  vtkSmartPointer< vtkPolyData > src =
    vtkSmartPointer< vtkPolyData >::Take(source());
  PolyViewMainWindow mw(src, &app);
  mw.show();
  return app.exec();
}
