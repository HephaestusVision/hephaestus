/**
  @file Parameters.h

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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QDialog>
#include <QSettings>
#include <QMap>
#include <QString>
class QLineEdit;
class QGridLayout;

/**
   Dialog to change parameters.  Subclass of QDialog
 */
class Parameters: public QDialog {
  Q_OBJECT; // QT Meta-object compiler macro.
private:
  QSettings settings;
  QMap<QString, QString> defaults;
  QMap<QString, QLineEdit*> inputWidgets;
  QGridLayout * gridLayout;
  int number_of_defaults;
public:
/**
   Constructor.  

   Settings will persisst across sessions.

   Settings will be stored in 
   ${HOME}/.config/${organization}/${application}.conf
   on Linux systems.

   Example:
   
   Parameters p("HephaestusVision", "HephaestusScanner");
   p.setDefault("Height","1");
   p.setDefault("Width","1");
   p.show();
   p.raise();
   p.activateWindow();
   //...///
   int height = p.getParameter("Height").toInt();
   int wifht = p.getParameter("Width").toInt();

   @param parent passed on to superclass.
 */
  Parameters(
    const QString & organization, 
    const QString & application,
    QWidget * parent=NULL);
  /**
     Changes the value of a parameter.  Should change on disc and in the
     dialog as well.  If there is no default, value becomes the new
     default.
  */
  void setParameter(const QString & key, const QString & value);
  /**
     Returns the value. If not set, the default is set.
  */
  QString getParameter(const QString & key) const;
  /**
     Returns the value converted to a double. If not set, the default is set.
  */
  double getValue(const QString & key) const
  {
    return this->getParameter(key).toDouble();
  }
  /**
     Adds a new parameter with a default value or changes the
     default. If the parameter is not new, the setting isn't changed,
     just the default.
  */
  void setDefault(const QString & key, const QString & value);
  void setDefault(const QString & key, double value)
  {
    this->setDefault(key, QString::number(value, 'g', 15));
  }
public slots:
/**
   Clears all settings, even those sotred on your hard drive.
   Returns everything to the defaults.   
 */
  void resetToDefaults();
  void process_reject();
  void process_accept();
signals:
  void changed();
};

#endif /* PARAMETERS_H */
