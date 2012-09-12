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
#include "PolyViewMainWindow.h"
#include <QObject>
#include <QToolBar>
#include <QAction>
#include <QApplication>

PolyViewMainWindow::PolyViewMainWindow(
  PolyDataSourceFunction srcFn, QApplication * app):
    srcFn(srcFn),
    currentSource((*(this->srcFn))()),
    polyWidget(this->currentSource)
{
  this->setCentralWidget(&(this->polyWidget));
  QToolBar * tool = this->addToolBar(QString("Tools"));
  
  QAction * exit_action = new QAction(QString("E&xit"), this);
  exit_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
  exit_action->setStatusTip("Exit application. Ctrl-q");
  exit_action->setToolTip("Exit application. Ctrl-q");
  QObject::connect(exit_action, SIGNAL(triggered()),app,SLOT(quit()));
  tool->addAction(exit_action);
  
  QAction * replace_action = new QAction(QString("&Replace"), this);
  replace_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  replace_action->setStatusTip("Exit application. Ctrl-q");
  replace_action->setToolTip("Exit application. Ctrl-q");
  QObject::connect(replace_action, SIGNAL(triggered()),this,
                   SLOT(replaceSrc()));
  tool->addAction(replace_action);
  
  this->setWindowTitle("Hello, QT-VTK!");
  this->adjustSize();
  this->show();
}
PolyViewMainWindow::~PolyViewMainWindow()
{
  this->currentSource->Delete();
}

void PolyViewMainWindow::replaceSrc()
{
  this->currentSource->Delete();
  this->currentSource = (*(this->srcFn))();
  this->polyWidget.newSource(this->currentSource);
}
