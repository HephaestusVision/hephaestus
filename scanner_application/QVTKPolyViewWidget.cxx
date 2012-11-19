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
#include "QVTKPolyViewWidget.h"

#include <cassert>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
//#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
#define setInputData(x,y) ((x)->SetInput(y))
#else
#define setInputData(x,y) ((x)->SetInputData(y))
#endif

QVTKPolyViewWidget::QVTKPolyViewWidget(QWidget *parent, vtkPolyData * src) :
	QVTKWidget(parent)
{
    this->mapper = vtkPolyDataMapper::New();
    this->actor = vtkActor::New();
    this->renderWindow = vtkRenderWindow::New();
    this->renderer = vtkRenderer::New();
    this->polySource = src;
    setInputData(this->mapper, this->polySource);
    this->actor->SetMapper(this->mapper);
    this->renderWindow->AddRenderer(this->renderer);
    this->renderer->AddActor(this->actor);

    this->renderer->ResetCamera();
    this->resize(480, 480);
    this->setMinimumSize(480, 480);
    this->SetRenderWindow(renderWindow);
    this->show();
}
QVTKPolyViewWidget::~QVTKPolyViewWidget()
{
    this->renderWindow->Delete();
    this->renderer->Delete();
    this->actor->Delete();
    this->mapper->Delete();
}

void QVTKPolyViewWidget::SetCamera(const double focalPoint[3], const double position[3], const double viewUp[3], double viewAngle)
{
  vtkCamera * camera = this->renderer->GetActiveCamera();
  assert(camera != NULL);
  camera->SetFocalPoint(focalPoint);
  camera->SetPosition(position);
  camera->SetViewUp(viewUp);
  camera->SetViewAngle(viewAngle);

}

void QVTKPolyViewWidget::GetCamera(double focalPoint[3], double position[3], double viewUp[3], double * viewAngle)
{
  vtkCamera * camera = this->renderer->GetActiveCamera();
  assert(camera != NULL);
  camera->GetFocalPoint(focalPoint);
  camera->GetPosition(position);
  camera->GetViewUp(viewUp);
  *viewAngle = camera->GetViewAngle();
}

void QVTKPolyViewWidget::ResetCamera()
{
	this->renderer->ResetCamera();
}


void QVTKPolyViewWidget::newSource(vtkPolyData * newPolySource)
{
	this->polySource = newPolySource;
  setInputData(this->mapper, this->polySource);
  this->renderWindow->Render();
}

void QVTKPolyViewWidget::saveVTK(QString filename, vtkPolyData * polyData)
{
  QByteArray filenameByteArray = filename.toUtf8();
  if (filename.endsWith(".vtp") || filename.endsWith(".VTP"))
    {
    vtkSmartPointer< vtkXMLPolyDataWriter > writer =
      vtkSmartPointer< vtkXMLPolyDataWriter >::New();
    writer->SetFileName(filenameByteArray.constData());
    setInputData(writer, polyData);
    writer->Update();
    }
  else if (filename.endsWith(".vtk") || filename.endsWith(".VTK"))
    {
    vtkSmartPointer< vtkPolyDataWriter > writer =
      vtkSmartPointer< vtkPolyDataWriter >::New();
    writer->SetFileName(filenameByteArray.constData());
    setInputData(writer, polyData);
    writer->Update();
    }
}
void QVTKPolyViewWidget::openVTK(QString filename, vtkPolyData * polyData)
{
  QByteArray filenameByteArray = filename.toUtf8();
  if (filename.endsWith(".vtp") || filename.endsWith(".VTP"))
    {
    vtkSmartPointer< vtkXMLPolyDataReader > reader =
      vtkSmartPointer< vtkXMLPolyDataReader >::New();
    reader->SetFileName(filenameByteArray.constData());
    reader->Update();
    polyData->ShallowCopy(reader->GetOutput());
    }
  else if (filename.endsWith(".vtk") || filename.endsWith(".VTK"))
    {
    vtkSmartPointer< vtkPolyDataReader > reader =
      vtkSmartPointer< vtkPolyDataReader >::New();
    reader->SetFileName(filenameByteArray.constData());
    reader->Update();
    polyData->ShallowCopy(reader->GetOutput());
    }
  else
    {
    //alert(NULL,"huh?");
    }
}
