/*
  Hephaestus Scanner Application / main.cxx

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

#include <QApplication>
#include "DepthScannerMainWindow.h"

int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  DepthScannerMainWindow mw(&app);
  mw.show();
  return app.exec();
}
