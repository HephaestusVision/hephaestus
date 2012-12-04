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

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <vector>
#include <QObject>
struct _IplImage;
class vtkPolyData;
class Parameters;
struct cloudy_configuration;

/**
  Cloudy is the glue between the Computer Vision code and the GUI.
*/
class Cloudy : public QObject {
  Q_OBJECT; // QT Meta-object compiler macro.
public:
  /** Constructor */
  Cloudy(Parameters * parameters = NULL);
  virtual ~Cloudy();

  // /**
  //    @return a pointer to the current point cloud.  Can return NULL if
  //    there are no point clouds right now.
  // */
  // PointCloud const * GetCurrentPointCloud() const;

  /**
     Save the current PointCloud object into a vtkPolyData.
  */
  void GetCurrentPointCloud(vtkPolyData * output);

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

public slots:
  void parametersChanged();

private:
  Cloudy(const Cloudy & c); // noncopyable resource
  Cloudy& operator=(const Cloudy & c); // noncopyable resource

  /* private instance variables */
  bool m_isGood;
  pcl::PointCloud<pcl::PointXYZRGBNormal> * pointCloud;
  Parameters * parameters;
  cloudy_configuration * config;
};

/**
   Function to convert to vtkPolyData. Overwrites output object.

   Example useage:
   \code
   void f(pcl::PointCloud<pcl::PointXYZRGBNormal> * cloud)
   {
     vtkSmartPointer< vtkPolyData > myVtkPolyData =
       vtkSmartPointer< vtkPolyData >::New();
     pointCloudToVtkPolyData(cloud,myVtkPolyData);
     do_something_with(myVtkPolyData);
   }
   \endcode
 */
int point_cloud_to_vtkPolyData(
  pcl::PointCloud<pcl::PointXYZRGBNormal> * cloud,
  vtkPolyData * output);

/**
   Function to convert from vtkPolyData to a
   pcl::PointCloud<pcl::PointXYZRGBNormal>.  Will get color
   information from the rgb_colors array if present.

   The returned pointer will point to a object created on the stack.
   it is the caller's repsonsibility to delete the object.

   Returns NULL on an error.
  */
pcl::PointCloud<pcl::PointXYZRGBNormal> * vtkPolyData_to_point_cloud(
  vtkPolyData * poly);


#endif /* CLOUDY_H */
