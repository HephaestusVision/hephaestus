/*
  Hephaestus Scanner Application / PickFolder.cxx

  Copyright 2012 University of North Carolina at Chapel Hill.

  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/

  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License int he file LICENSE.txt in this
  repository or at:
    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.
*/

#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QString>
#include <QSizePolicy>
#include <QList>

#include "MidasFolder.h"
#include "PickFolder.h"

PickFolder::PickFolder(
  QWidget * parent, QList< MidasFolder > * list_of_folders) :
  QDialog(parent),
  list_of_folders(list_of_folders),
  selectedFolder(NULL)
{
  QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);
  this->verticalLayout = new QVBoxLayout(this);
  this->comboBox = new QComboBox(this);
  QSizePolicy comboBoxSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  comboBoxSizePolicy.setHorizontalStretch(0);
  comboBoxSizePolicy.setVerticalStretch(0);
  comboBoxSizePolicy.setHeightForWidth(
    this->comboBox->sizePolicy().hasHeightForWidth());
  this->comboBox->setSizePolicy(comboBoxSizePolicy);
  this->verticalLayout->addWidget(this->comboBox);

  this->buttonBox = new QDialogButtonBox(this);
  this->buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  this->buttonBox->setOrientation(Qt::Horizontal);
  this->buttonBox->setStandardButtons(
    QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  this->verticalLayout->addWidget(this->buttonBox);

  for (int i = 0; i < list_of_folders->size(); ++i)
    {
    this->comboBox->addItem(list_of_folders->at(i).name, QVariant(i));
    // remember the index of the folder.
    }
  QObject::connect(
    this->buttonBox, SIGNAL(accepted()), this, SLOT(process_accept()));
  QObject::connect(
    this->buttonBox, SIGNAL(rejected()), this, SLOT(process_reject()));
}

const MidasFolder * PickFolder::getFolder() const{
  return this->selectedFolder;
}

void PickFolder::process_reject() {
  this->selectedFolder = NULL;
  this->reject();
}
void PickFolder::process_accept()
{
  int idx = this->comboBox->itemData(this->comboBox->currentIndex()).toInt();
  this->selectedFolder = &(this->list_of_folders->at(idx));
  this->accept();
}

