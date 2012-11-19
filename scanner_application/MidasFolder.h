/**
  @file MidasFolder.h

  @authors
  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/
    
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


// #include "MidasFolder.h"
#ifndef MIDASFOLDER_H
#define MIDASFOLDER_H

#include <QString>

/**
  MidasFolder is a simple class to represent a folder on a server.  Included is all the necessary information to access the folder.
*/
class MidasFolder {
public:
	/**
	 The server's address */
  QString server;
	/**
	 A login session token */
  QString token;
	/**
	 Folder ID */
  QString id;
	/**
	 Folder name */
  QString name;
	/**
	  */
  bool is_community_folder;
	/**
		 Default Constructor */
  MidasFolder(
    QString & server, QString & token, QString & id,
    QString & name, bool is_community_folder) :
    server(server), token(token), id(id), name(name),
    is_community_folder(is_community_folder) { }
};

#endif /* MIDASFOLDER_H */
