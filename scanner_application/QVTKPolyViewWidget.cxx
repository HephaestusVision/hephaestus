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

QVTKPolyViewWidget::QVTKPolyViewWidget(vtkPolyData * src)
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
    // this->polySource->Delete(); // don't take ownership of source
}
void QVTKPolyViewWidget::newSource(vtkPolyData * newPolySource)
{
    setInputData(this->mapper, newPolySource);
    this->renderWindow->Render();
    // this->polySource->Delete(); // don't take ownership of source
}
