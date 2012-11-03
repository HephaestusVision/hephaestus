/*
  Hephaestus Scanner Application /

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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QString>

/**
 */

class LoginDialog: public QDialog {
  Q_OBJECT // QT Meta-object compiler macro.
private:
  QLineEdit * lineEdit_server;  // "http://dpbld04a.cs.unc.edu/midas/api/json"
  QLineEdit * lineEdit_appname; // "scanner"
  QLineEdit * lineEdit_email;   // "halcanary@gmail.com"
  QLineEdit * lineEdit_apikey;  // "TUZvgO8aDYbv9NkDgUPrGeWMD6vgxCmhmcCsQAsj" 
	QLineEdit * lineEdit_descriptor;
  bool m_isGood;
public:
  LoginDialog(QWidget * parent);
  QString server()  const { return this->lineEdit_server->text();  } 
  QString appname() const { return this->lineEdit_appname->text(); } 
  QString email()   const { return this->lineEdit_email->text();   } 
  QString apikey()  const { return this->lineEdit_apikey->text();  } 
	QString descriptor() const { return this->lineEdit_descriptor->text();  } 
  bool isGood()  const { return this->m_isGood;  } 
  void setServer(QString &s)  { this->lineEdit_server->setText(s);  }
  void setAppname(QString &s) { this->lineEdit_appname->setText(s); }
  void setEmail(QString &s)   { this->lineEdit_email->setText(s);   }
  void setApikey(QString &s)  { this->lineEdit_apikey->setText(s);  }
  void setDescriptor(QString &s)  { this->lineEdit_descriptor->setText(s);  }
public slots:
  void process_reject();
  void process_accept();
};

#endif /* LOGINDIALOG_H */
