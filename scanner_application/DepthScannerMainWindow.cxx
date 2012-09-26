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
#include <QObject>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QHBoxLayout>

#include <vtkPolyData.h>

#include "DepthScannerMainWindow.h"
#include "Cloudy.h"
#include "PointCloud.h"
#include "CameraWidget.h"

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

static void alert(QWidget * parent, const char * s)
{
	QMessageBox::about(parent, QString(s), QString(s));
}

static void updatePC(DepthScannerMainWindow * dsmw)
{
  const PointCloud * pc = dsmw->cloudy->GetCurrentPointCloud();
	if (pc == NULL)
		return;
	convertPointCloudToVtkPoly(pc, dsmw->pointCloud);
  dsmw->qVTKPolyViewWidget.newSource(dsmw->pointCloud);
}

void DepthScannerMainWindow::create()
{
  this->cloudy->ClearPointCloud();
	this->cloudy->CreatePointCloud();
	updatePC(this);
	//alert(this, "create");
}


void DepthScannerMainWindow::update()
{
  this->cloudy->UpdatePointCloud();
	updatePC(this);
	//	alert(this, "update");
}

void DepthScannerMainWindow::save()
{
	alert(this, "save");
}

void DepthScannerMainWindow::load()
{
	alert(this, "load");
}

DepthScannerMainWindow::DepthScannerMainWindow(QApplication * app):
	qApplication(app),
	pointCloud(vtkPolyData::New()),
	qVTKPolyViewWidget(pointCloud),
	cloudy(new Cloudy())
{
  QToolBar * tool = this->addToolBar(QString("Tools"));

  tool->addAction(
    createAction("E&xit", app, QKeySequence(Qt::CTRL + Qt::Key_Q),
      "Exit application. Ctrl-q", SIGNAL(triggered()), SLOT(quit())));

  tool->addAction(
    createAction("&Create", this, QKeySequence(Qt::CTRL + Qt::Key_N),
      "create a new cloud", SIGNAL(triggered()), SLOT(create())));

  tool->addAction(
    createAction("&Update", this, QKeySequence(Qt::CTRL + Qt::Key_U),
      "add points to the point cloud.", SIGNAL(triggered()), SLOT(update())));

  tool->addAction(
    createAction("&Save", this, QKeySequence(Qt::CTRL + Qt::Key_S),
      "save to file", SIGNAL(triggered()), SLOT(save())));

  tool->addAction(
    createAction("&Load", this, QKeySequence(Qt::CTRL + Qt::Key_S),
      "load from file", SIGNAL(triggered()), SLOT(load())));

  QWidget * centralwidget = new QWidget(this);
  QHBoxLayout * horizontalLayout = new QHBoxLayout(centralwidget);
  horizontalLayout->setContentsMargins(0, 0, 0, 0);

	this->camera = new CameraWidget();
	this->camera->resize(400,400);
	this->camera->setMinimumSize (400,400);

  horizontalLayout->addWidget(this->camera);
  horizontalLayout->addWidget(&qVTKPolyViewWidget);
  this->setCentralWidget(centralwidget);

  this->setWindowTitle("Hephaestus");

  this->adjustSize();
  this->show();
}


DepthScannerMainWindow::~DepthScannerMainWindow()
{
	this->pointCloud->Delete();
	delete this->camera;
	delete this->cloudy;
}
