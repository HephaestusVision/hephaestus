/*
  DepthScannerMainWindow.cxx

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
#include <cstdlib>

#include <QObject>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QTemporaryFile>
#include <QGroupBox>

#include <vtkPolyData.h>

#include "DepthScannerMainWindow.h"
#include "Cloudy.h"
#include "PointCloud.h"
#include "CameraWidget.h"
#include "MidasUpload.h"

static QAction * createAction(
  char const * name,
  QObject * receiver,
  const QKeySequence & shortcut,
  char const * tip,
  const char * signal,
  const char * slot)
{
  QAction * action = new QAction(QString(name), receiver);
  action->setShortcut(shortcut);
  action->setStatusTip(tip);
  action->setToolTip(tip);
  QObject::connect(action, signal, receiver, slot);
  return action;
}

static inline void alert(QWidget * parent, QString s)
{
  QMessageBox::about(parent, s, s);
}
static inline void alert(QWidget * parent, const char * s)
{
  alert(parent, QString(s));
}

static void updatePC(DepthScannerMainWindow * dsmw)
{
  const PointCloud * pc = dsmw->cloudy->GetCurrentPointCloud();
  if (pc == NULL)
    {
    alert(dsmw, "NULL CurrentPointCloud");
    return;
    }
  convertPointCloudToVtkPoly(pc, dsmw->pointCloud);
  dsmw->qVTKPolyViewWidget.newSource(dsmw->pointCloud);
}

void DepthScannerMainWindow::upload()
{
  QTemporaryFile tempfile ( "hephaestus_temp_XXXXXX.vtp" );
  if (! tempfile.open()) {
    std::cerr << "error creating QTemporaryFile\n\n";
    return;
  }
  QString fileName = tempfile.fileName();
  tempfile.close();
  QVTKPolyViewWidget::saveVTK(fileName, this->pointCloud);
  upload_to_midas(&(this->loginDialog), fileName);
}
void DepthScannerMainWindow::create()
{
  this->cloudy->ClearPointCloud();
  this->cloudy->CreatePointCloud();
  updatePC(this);
  double FocalPoint[3] = {0.0, 0.0, 1.0};
  double position[3] = {0.0, 0.0, -0.0};
  double viewUp[3] = {0.0, -1.0, 0.0};
  double viewAngle = 45.0;
  this->qVTKPolyViewWidget.SetCamera(FocalPoint, position, viewUp, viewAngle);
}


void DepthScannerMainWindow::update()
{
  this->cloudy->UpdatePointCloud();
  updatePC(this);
}


void DepthScannerMainWindow::save()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr("Save VTK File"), "", tr("VTK Files (*.vtk *.vtp)"));
  QVTKPolyViewWidget::saveVTK(fileName, this->pointCloud);
}


void DepthScannerMainWindow::load()
{
  QString fileName = QFileDialog::getOpenFileName(this,
     tr("Open VTK File"), "", tr("VTK Files (*.vtk *.vtp)"));
  QVTKPolyViewWidget::openVTK(fileName, this->pointCloud);

  double FocalPoint[3] = {0.0, 0.0, 1.0};
  double position[3] = {0.0, 0.0, -0.0};
  double viewUp[3] = {0.0, -1.0, 0.0};
  double viewAngle = 45.0;
  this->qVTKPolyViewWidget.SetCamera(FocalPoint, position, viewUp, viewAngle);
  //void SetCamera(const double FocalPoint[3], const double position[3], const double viewUp[3]);
  // double FocalPoint[3];
  // double position[3];
  // double viewUp[3];
  // this->qVTKPolyViewWidget.GetCamera(FocalPoint, position, viewUp);
  // std::cout
  //   << FocalPoint[0] << '\t' << FocalPoint[1] << '\t' << FocalPoint[2] << '\n'
  //   << position[0] << '\t' << position[1] << '\t' << position[2] << '\n'
  //   << viewUp[0] << '\t' << viewUp[1] << '\t' << viewUp[2] << '\n';
}

DepthScannerMainWindow::DepthScannerMainWindow():
  QMainWindow(),
  pointCloud(vtkPolyData::New()),
  qVTKPolyViewWidget(pointCloud),
  cloudy(new Cloudy()),
  loginDialog(this)
{
  QToolBar * tool = this->addToolBar(QString("Tools"));

  // tool->addAction(
  //   createAction("E&xit", app, QKeySequence(Qt::CTRL + Qt::Key_Q),
  //     "Exit application. Ctrl-q", SIGNAL(triggered()), SLOT(quit())));
  tool->addAction(
    createAction("E&xit", this, QKeySequence(Qt::CTRL + Qt::Key_Q),
      "Exit application. Ctrl-q", SIGNAL(triggered()), SLOT(close())));

  tool->addAction(
    createAction("&Create", this, QKeySequence(Qt::CTRL + Qt::Key_N),
      "create a new cloud. Ctrl-n ", SIGNAL(triggered()), SLOT(create())));

  tool->addAction(
    createAction("&Update", this, QKeySequence(Qt::CTRL + Qt::Key_U),
      "add points to the point cloud. Ctrl-u", SIGNAL(triggered()), SLOT(update())));

  tool->addAction(
    createAction("&Save", this, QKeySequence(Qt::CTRL + Qt::Key_S),
      "save to file. Ctrl-s", SIGNAL(triggered()), SLOT(save())));

  tool->addAction(
    createAction("&Load", this, QKeySequence(Qt::CTRL + Qt::Key_L),
      "load from file.  Ctrl-l", SIGNAL(triggered()), SLOT(load())));

  tool->addAction(
    createAction("U&pload", this, QKeySequence(Qt::CTRL + Qt::Key_P),
      "upload to server.  Ctrl-p", SIGNAL(triggered()), SLOT(upload())));

  QWidget * centralwidget = new QWidget(this);
  QHBoxLayout * horizontalLayout = new QHBoxLayout(centralwidget);
  horizontalLayout->setContentsMargins(0, 0, 0, 0);

  const int camera_width = 320; // half-size camera
  const int camera_height = 240;

  this->camera = new CameraWidget();
  this->camera->resize(camera_width,camera_height);
  this->camera->setMinimumSize(camera_width,camera_height);
  this->camera->setMaximumSize(camera_width,camera_height);

  const char CAMERA_LABEL [] = "Camera View";
  QGroupBox * cameraBox = new QGroupBox(this);
  cameraBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  cameraBox->setTitle(QString::fromUtf8(CAMERA_LABEL));
  QHBoxLayout * horizontalLayout2 = new QHBoxLayout(cameraBox);
  horizontalLayout2->addWidget(this->camera);

  horizontalLayout->addWidget(cameraBox);
  horizontalLayout->addWidget(&qVTKPolyViewWidget);
  this->setCentralWidget(centralwidget);

  this->setWindowTitle("Hephaestus");

  this->adjustSize();
  this->show();

  // double FocalPoint[3] = {0.0, 0.0, 1.0};
  // double position[3] = {0.0, 0.0, 0.0};
  // double viewUp[3] = {0.0, -1.0, 0.0};
  // this->qVTKPolyViewWidget.SetCamera(FocalPoint, position, viewUp);

  // char * infinity = std::getenv ( "HEPHAESTUS_INFINITY" ); //millimeters
  // short v = (infinity != NULL) ? static_cast<short>(atoi(infinity)) : 0;
  // if (v > 0)
  //  this->cloudy->setMaximimDepth(v);

}

DepthScannerMainWindow::~DepthScannerMainWindow()
{
  this->pointCloud->Delete();
  delete this->camera;
  delete this->cloudy;
}


