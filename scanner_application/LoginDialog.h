/**
  @file LoginDialog.h

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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QString>

/**
   Dialog to ask for server URL, appname, email, apikey, and file
   descriptor.  Subclass of QDialog
 */
class LoginDialog: public QDialog {
  Q_OBJECT // QT Meta-object compiler macro.
private:
  QLineEdit * lineEdit_server;  // "http://dpbld04a.cs.unc.edu/midas/api/json"
  QLineEdit * lineEdit_appname; // "scanner"
  QLineEdit * lineEdit_email;   // "halcanary@gmail.com"
  QLineEdit * lineEdit_apikey;  // "TUZvgO8aDYbv9NkDgUPrGeWMD6vgxCmhmcCsQAsj" 
  QLineEdit * lineEdit_descriptor; // "filename.vtp"
  bool m_isGood;
public:
  /**
     Constructor.  
     @param parent passed on to superclass. */
  LoginDialog(QWidget * parent=NULL);
  /**
     After the dialog closes, use these methods to retrieve the
     information the user input.
   */
  QString server()  const { return this->lineEdit_server->text();  } 
  /**
     After the dialog closes, use these methods to retrieve the
     information the user input.
   */
  QString appname() const { return this->lineEdit_appname->text(); } 
  /**
     After the dialog closes, use these methods to retrieve the
     information the user input.
   */
  QString email()   const { return this->lineEdit_email->text();   } 
  /**
     After the dialog closes, use these methods to retrieve the
     information the user input.
   */
  QString apikey()  const { return this->lineEdit_apikey->text();  } 
  /**
     After the dialog closes, use these methods to retrieve the
     information the user input.
   */
  QString descriptor() const { return this->lineEdit_descriptor->text();  } 
  /**
     @return did the user click "OK"?
   */
  bool isGood()  const { return this->m_isGood;  } 
  /**
     Programaticcly set the server field.
   */
  void setServer(QString &s)  { this->lineEdit_server->setText(s);  }
  /**
     Programaticcly set the Appname field.
   */
  void setAppname(QString &s) { this->lineEdit_appname->setText(s); }
  /**
     Programaticcly set the email field.
   */
  void setEmail(QString &s)   { this->lineEdit_email->setText(s);   }
  /**
     Programaticcly set the apikey field.
   */
  void setApikey(QString &s)  { this->lineEdit_apikey->setText(s);  }
  /**
     Programaticcly set the descriptor field.
   */
  void setDescriptor(QString &s)  { this->lineEdit_descriptor->setText(s);  }
public slots:
  void process_reject();
  void process_accept();
};

#endif /* LOGINDIALOG_H */
