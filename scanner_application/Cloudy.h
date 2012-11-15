/*
  Cloudy.h

  Copyright 2012 University of North Carolina at Chapel Hill.

  Written 2012 by Hal Canary, Christopher Leslie, Frank Ferro
    http://hephaestusvision.github.com/hephaestus/

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

  * * *

  Cloudy is the glue between the Computer Vision code and the GUI.
*/

#ifndef CLOUDY_H
#define CLOUDY_H

class PointCloud;
struct _IplImage;
class Cloudy {
public:
  Cloudy(/* FIXME: constructor arguments? */);
  virtual ~Cloudy();
  PointCloud const * GetCurrentPointCloud() const;
  void UpdatePointCloud();
  void CreatePointCloud();
  void ClearPointCloud();
  bool isGood();
	/** units of milimeters */
	short MaximimDepth();
	/** units of milimeters */
	void setMaximimDepth(short);
private:
  Cloudy(const Cloudy & c); // noncopyable resource
  Cloudy& operator=(const Cloudy & c); // noncopyable resource

  /* private instance variables */
  bool m_isGood;
	short m_MaximimDepth;
  _IplImage * rgbImage;
  _IplImage * depthImage;
  PointCloud * pcloud;
};

#endif /* CLOUDY_H */
