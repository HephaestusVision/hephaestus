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
#include "CameraWidget.h"
#include "MidasUpload.h"

static const char CAMERA_LABEL [] = "Camera View";
static const char POLYVIEW_LABEL [] = "Rendered Object";
static const char WINDOW_TITLE [] = "Hephaestus Scanner";


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

void DepthScannerMainWindow::upload()
{
  QTemporaryFile tempfile ( "hephaestus_temp_XXXXXX.vtp" );
  if (! tempfile.open()) {
    std::cerr << "error creating QTemporaryFile\n\n";
    return;
  }
  QString fileName = tempfile.fileName();
  QString suffix(".vtp");
  tempfile.close();
  QVTKPolyViewWidget::saveVTK(fileName, this->pointCloud);
  upload_to_midas(&(this->loginDialog), fileName, suffix);
}

void DepthScannerMainWindow::settings()
{
  this->parameters.show();
  this->parameters.raise();
  this->parameters.activateWindow();
}

static void resetCamera(QVTKPolyViewWidget * qVTKPolyViewWidget)
{
  double FocalPoint[3] = {0.0, 0.0, 1.0};
  double position[3] = {0.0, 0.0, -0.0};
  double viewUp[3] = {0.0, -1.0, 0.0};
  double viewAngle = 45.0;
  qVTKPolyViewWidget->SetCamera(FocalPoint, position, viewUp, viewAngle);
  qVTKPolyViewWidget->ResetCamera();
}

void DepthScannerMainWindow::create()
{
  //this->cloudy->ClearPointCloud(); // assumed to be not necessary.
  this->cloudy->CreatePointCloud();
  this->cloudy->GetCurrentPointCloud(this->pointCloud);
  this->qVTKPolyViewWidget.newSource(this->pointCloud);
  resetCamera(&(this->qVTKPolyViewWidget));
}

void DepthScannerMainWindow::revert()
{
  this->cloudy->RevertPointCloud();
  this->cloudy->GetCurrentPointCloud(this->pointCloud);
  this->qVTKPolyViewWidget.newSource(this->pointCloud);
}

void DepthScannerMainWindow::update()
{
  this->cloudy->UpdatePointCloud();
  this->cloudy->GetCurrentPointCloud(this->pointCloud);
  this->qVTKPolyViewWidget.newSource(this->pointCloud);
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

  double FocalPoint[3] = {0.0, 0.0, 0.1};
  double position[3] = {0.0, 0.0, 0.0};
  double viewUp[3] = {0.0, -1.0, 0.0};
  double viewAngle = 45.0;
  this->qVTKPolyViewWidget.SetCamera(FocalPoint, position, viewUp, viewAngle);
  this->qVTKPolyViewWidget.ResetCamera();
  // this->qVTKPolyViewWidget.GetCamera(
  //   FocalPoint, position, viewUp, &viewAngle);
  // std::cout
  //   << FocalPoint[0] << '\t' << FocalPoint[1]
  //   << '\t' << FocalPoint[2] << '\n'
  //   << position[0] << '\t' << position[1]
  //   << '\t' << position[2] << '\n'
  //   << viewUp[0] << '\t' << viewUp[1]
  //   << '\t' << viewUp[2] << '\n'
  //   << viewAngle;  
}

DepthScannerMainWindow::DepthScannerMainWindow():
  QMainWindow(),
  parameters("HephaestusVision", "HephaestusScanner"),
  pointCloud(vtkPolyData::New()),
  qVTKPolyViewWidget(this, pointCloud),
  cloudy(new Cloudy(&parameters)),
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
    createAction("Revert", this, QKeySequence(Qt::CTRL + Qt::Key_Z),
      "revert the point cloud to previous state. Ctrl-z",
      SIGNAL(triggered()), SLOT(revert())));

  tool->addAction(
    createAction("&Save", this, QKeySequence(Qt::CTRL + Qt::Key_S),
      "save to file. Ctrl-s", SIGNAL(triggered()), SLOT(save())));

  tool->addAction(
    createAction("&Load", this, QKeySequence(Qt::CTRL + Qt::Key_L),
      "load from file.  Ctrl-l", SIGNAL(triggered()), SLOT(load())));

  tool->addAction(
    createAction("U&pload", this, QKeySequence(Qt::CTRL + Qt::Key_P),
      "upload to server.  Ctrl-p", SIGNAL(triggered()), SLOT(upload())));

  tool->addAction(
    createAction("Settings", this, QKeySequence(Qt::CTRL + Qt::Key_T),
      "Change settings.  Ctrl-t", SIGNAL(triggered()), SLOT(settings())));

  QWidget * centralwidget = new QWidget(this);
  QHBoxLayout * horizontalLayout = new QHBoxLayout(centralwidget);
  horizontalLayout->setContentsMargins(0, 0, 0, 0);

  const int camera_widget_width = 320; // half-size camera
  const int camera_widget_height = 240;

  this->camera = new CameraWidget();
  this->camera->resize(camera_widget_width,camera_widget_height);
  this->camera->setMinimumSize(camera_widget_width,camera_widget_height);
  this->camera->setMaximumSize(camera_widget_width,camera_widget_height);

  QGroupBox * cameraBox = new QGroupBox(this);
  cameraBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  cameraBox->setTitle(QString::fromUtf8(CAMERA_LABEL));
  QHBoxLayout * horizontalLayout2 = new QHBoxLayout(cameraBox);
  horizontalLayout2->addWidget(this->camera);


  QGroupBox * qVTKPolyViewBox = new QGroupBox(this);
  // qVTKPolyViewBox->setSizePolicy(
  //   QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  qVTKPolyViewBox->setTitle(QString::fromUtf8(POLYVIEW_LABEL));
  QHBoxLayout * horizontalLayout3 = new QHBoxLayout(qVTKPolyViewBox);
  horizontalLayout3->addWidget(&(this->qVTKPolyViewWidget));

  horizontalLayout->addWidget(cameraBox);
  horizontalLayout->addWidget(qVTKPolyViewBox);
  //horizontalLayout->addWidget(&(this->qVTKPolyViewWidget));
  this->setCentralWidget(centralwidget);

  this->setWindowTitle(WINDOW_TITLE);

  this->adjustSize();
  this->show();

  parameters.setDefault("Default Server", "");
  parameters.setDefault("Default Appname", "");
  parameters.setDefault("Default Email", "");
  parameters.setDefault("Default APIKey", "");
  QString default_server  = parameters.getParameter("Default Server") ;
  QString default_appname = parameters.getParameter("Default Appname");
  QString default_email   = parameters.getParameter("Default Email") ; 
  QString default_apikey  = parameters.getParameter("Default APIKey") ;
  this->loginDialog.setServer( default_server );
  this->loginDialog.setAppname(default_appname);
  this->loginDialog.setEmail(  default_email  );
  this->loginDialog.setApikey( default_apikey );
}

DepthScannerMainWindow::~DepthScannerMainWindow()
{
  this->pointCloud->Delete();
  delete this->camera;
  delete this->cloudy;
}


