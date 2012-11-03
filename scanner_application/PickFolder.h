/*
  Hephaestus Scanner Application / PickFolder.h

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

#ifndef PICKFOLDER_H
#define PICKFOLDER_H

#include "MidasFolder.h"

#include <QDialog>
#include <QList>
class QVBoxLayout;
class QComboBox;
class QDialogButtonBox;

/**
	 This class is a dialog window that, given a list of MidasFolder
	 objects, asks the user which one he wants.  When the dialog is
	 finished, .getFolder() will return the correct a pointer to the
	 selected folder, or null.  It allocates no memory for folders, and
	 makes no copies of folders.

	 		PickFolder pick_folder(parent, &list_of_folders);
  		pick_folder.show();
  		pick_folder.raise();
  		pick_folder.activateWindow();
  
			app.exec(); or just wait.

    	const MidasFolder * folder = pick_folder.getFolder();

  		if (folder == NULL)
				...deal with errors..
 */

class PickFolder: public QDialog {
	Q_OBJECT // QT Meta-object compiler macro.
private:
  QList< MidasFolder > * list_of_folders;
  QVBoxLayout * verticalLayout;
  QComboBox * comboBox;
  QDialogButtonBox * buttonBox;
  const MidasFolder * selectedFolder;
public:
  PickFolder(QWidget * parent, QList< MidasFolder > * list_of_folders);
  const MidasFolder * getFolder() const;
public slots:
  void process_reject();
  void process_accept();
};

#endif /* PICKFOLDER_H */
