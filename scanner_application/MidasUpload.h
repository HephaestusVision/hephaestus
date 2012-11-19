/**
  @file MidasUpload.h

  Several functions to manage connections to a Midas
  server. http://www.midasplatform.org/

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

#ifndef MIDASUPLOAD_H
#define MIDASUPLOAD_H

#include <iostream>
class QString;
class QUrl;
class QVariant;
class MidasFolder;

/**
	 Given a string containing JSON data, return a tree structure.

	 @warning VERY UNSAFE.  DO NOT USE UNTRUSTED JSON!!!!
*/
QVariant parseJson(QByteArray & json);

/**
	 Given a URL, do a HTTP GET, and return the JSON data, parsed into a
	 tree structure.

	 @warning VERY UNSAFE.  DO NOT USE UNTRUSTED JSON!!!!
*/
QVariant getJson(const QUrl & url);

/**
	 Given a URL, do a HTTP PUT, and return the JSON data, parsed into a
	 tree structure.

	 @param filename will be open and sent as the PUT datastream.

	 @warning VERY UNSAFE.  DO NOT USE UNTRUSTED JSON!!!!
*/
QVariant putJson(const QUrl & url, const QString & filename);


/**
	 Debug function to print a QVariant tree to a outputstream.
*/
std::ostream & print(std::ostream & outputstream, QVariant const &, int n = 0);

/**
  checks the return status of a getJson call.  Failed calls whill
  return a invalid QVariant.  Then MIDAS api always returns a "stat"
  field which is set to "ok" if somethng goes wrong. 
 */
bool isokay(QVariant & v, QString what);

/**
   Given a set of login information, get a list of visible folders on
   the server.
*/
QList< MidasFolder > get_list_of_folders(
  QString server, QString appname,
  QString email, QString appkey);

/**
   waits till a dialog emits the finished() signal.  Used to ask for
	 user input and wait until they are done.
*/
void runDialog(QDialog * dialog);

/**
   perform an upload.
*/
bool upload_to_midas_folder(
  const MidasFolder * folder,
  QString & filename,
  QString & fileDescriptor,
  QString & file_suffix);

/**
	 The function you want to call.  Will determine where to upload from
	 user input and perform the upload.
*/
void upload_to_midas(
  LoginDialog * loginDialog, QString & filename, QString & file_suffix);


#endif /* MIDASUPLOAD_H */
