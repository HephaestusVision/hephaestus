/**
	@file Cloudy.h

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

#ifndef CLOUDY_H
#define CLOUDY_H

class PointCloud;
struct _IplImage;

/**
  Cloudy is the glue between the Computer Vision code and the GUI.
*/
class Cloudy {
public:
	/** Constructor */
  Cloudy(/* FIXME: constructor arguments? */);
  virtual ~Cloudy();
	/**
		 @return a pointer to the current point cloud.  Can return NULL if
		 there are no point clouds right now.
	*/
  PointCloud const * GetCurrentPointCloud() const;
	/**
		 Modify the current point cloud by adding points to it.
	*/
  void UpdatePointCloud();
	/**
		 Create a new point cloud.  Any old PC will be lost.
	*/
  void CreatePointCloud();

	/**
		 Reset to initial condition.
	*/
  void ClearPointCloud();

	/**
		 @return Does the freenect_sync_get_rgb_cv() function return
		 non-NULL value?  I.e. is the Kinect connected?
	*/
  bool isGood();
	/**
		 @return current maximum depth value in units of milimeters.
	 */
	short MaximimDepth();
	/**
		 @param depth_in_millimeters Set the maximum depth value in units
		 of milimeters.
	 */
	void setMaximimDepth(short depth_in_millimeters);
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
