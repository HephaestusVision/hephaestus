/**
  @file Parameters.cxx

  @authors
  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/
    
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

#include <cassert>
#include <iostream>

#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>

#include "Parameters.h"


Parameters::Parameters(
  const QString & organization, 
  const QString & application,
  QWidget * parent) :
    QDialog(parent),
    settings(organization, application),
    defaults(),
    inputWidgets(),
    verticalLayout(this),
    gridLayout(),
    buttonBox(),
    number_of_defaults(0)
{
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);

  this->verticalLayout.addLayout(&(this->gridLayout));
  this->buttonBox.setOrientation(Qt::Horizontal);
  this->buttonBox.setStandardButtons(
    QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  this->verticalLayout.addWidget(&(this->buttonBox));

  QObject::connect(
    &(this->buttonBox), SIGNAL(accepted()), this, SLOT(process_accept()));
  QObject::connect(
    &(this->buttonBox), SIGNAL(rejected()), this, SLOT(process_reject()));
}


void Parameters::resetToDefaults()
{
  this->settings.clear();
  QMapIterator<QString, QString> defaultsIterator(this->defaults);
  while (defaultsIterator.hasNext())
    {
    defaultsIterator.next();
    this->inputWidgets[defaultsIterator.key()]->setText(defaultsIterator.value());
    }
}

void Parameters::setParameter(const QString & key, const QString & value)
{
  if (! this->defaults.contains(key))
    this->setDefault(key, value);
  else
    this->inputWidgets[key]->setText(value);
  this->settings.setValue(key, QVariant(value));
}

QString Parameters::getParameter(const QString & key) const 
{
  if (this->settings.contains(key))
    return this->settings.value(key).toString();
  else if (this->defaults.contains(key))
    return this->defaults.value(key);
  else 
    return "";
}

void Parameters::setDefault(const QString & key, const QString & value)
{
  if (! this->defaults.contains(key))
    {
    this->defaults[key] = value;
    
    // create a entry in the dialog
    QString labelString(key);
    labelString += ": ";
    QLabel * label   = new QLabel(labelString);
    QLineEdit * widget = new QLineEdit();

    this->inputWidgets[key] = widget;
        
    label->setAlignment( Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    this->gridLayout.addWidget(label,  this->number_of_defaults, 0, 1, 1);
    this->gridLayout.addWidget(widget, this->number_of_defaults, 1, 1, 1);
    ++(this->number_of_defaults);
    std::cerr << "X\n";

    widget->setText(this->getParameter(key));
    }
  else
    {
    this->defaults[key] = value; // Just change it. Do nothing else.
    }
}


void Parameters::process_reject()
{
  QMapIterator<QString, QLineEdit*> inputIterator(this->inputWidgets);
  while (inputIterator.hasNext())
    {
    inputIterator.next();
    inputIterator.value()->setText(this->getParameter(inputIterator.key()));
    }
  this->reject();
}


void Parameters::process_accept() 
{
  QMapIterator<QString, QLineEdit*> inputIterator(this->inputWidgets);
  while (inputIterator.hasNext())
    {
    inputIterator.next();
    this->settings.setValue(inputIterator.key(), QVariant(inputIterator.value()->text()));
    }  
  this->accept();
}
