/*
  Cloudy.cxx

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
*/

#include "Cloudy.h"
#include "PointCloud.h"

#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>

#include <cv.h>
#include "libfreenect_cv.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <math.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;


static void alert(const char * s)
{
  if (QCoreApplication::instance() != NULL)
    QMessageBox::about(NULL, "Hephaestus", s);
  else
    std::cerr << s << '\n';
}

static const short DEFAULT_MAXIMIM_DEPTH = 2047;

Cloudy::Cloudy(/* FIXME: constructor arguments? */):
  m_isGood(false), m_MaximimDepth(DEFAULT_MAXIMIM_DEPTH),
  rgbImage(NULL), depthImage(NULL), pcloud(NULL) { }

Cloudy::~Cloudy()
{
  delete this->pcloud;
}

short Cloudy::MaximimDepth() { return this->m_MaximimDepth; }
void Cloudy::setMaximimDepth(short v) { this->m_MaximimDepth = v; }

bool Cloudy::isGood()
{
  return this->m_isGood;
}

PointCloud const * Cloudy::GetCurrentPointCloud() const
{
  return this->pcloud;
}


static PointCloud * depth_image_to_point_cloud(
  _IplImage const * rgbImage,
  _IplImage const * depthImage,
  short infinity)
{
  if (rgbImage == NULL || depthImage == NULL)
    return NULL;
  int rows = depthImage->height;
  int columns = depthImage->width;
  short *depthValues = (short*)(depthImage->imageData);
  int points = 0;
  // int maxDepth = -1;
  for (int i = 0; i < rows; ++i)
    {
    for (int j = 0; j < columns; ++j)
      {
      int depthVal = depthValues[(i*columns) + j];
      if (depthVal < infinity)
        {
        ++points;
        // if(depthVal > maxDepth)
        //   {
        //   maxDepth = depthVal;
        //   }
        }
      }
    }

  unsigned long int numTriangles = 0;
  PointCloud * pc =
    new PointCloud(static_cast<unsigned long int>(points),numTriangles);

  int point_index = 0;

  unsigned char * colorVals = (unsigned char*)(rgbImage->imageData);

  for (int i = 0; i < rows; ++i)
    {
    for (int j = 0; j < columns; ++j)
      {
      double depthVal = static_cast<double>(depthValues[(i * columns) + j]);
      if ((depthVal < infinity) && (point_index < (3 * points)))
        {
        double scaleFactor = 0.1; //100?
        double tmpVal1 = -0.0030711016;
        double tmpVal2 = 3.3309495161;
        double depthMeters = scaleFactor / (tmpVal1 * depthVal + tmpVal2);
        const double fx_d = 5.9421434211923247e+02;
        const double fy_d = 5.9104053696870778e+02;
        const double cx_d = 3.3930780975300314e+02;
        const double cy_d = 2.4273913761751615e+02;
        double xCoordinate = ((j - cx_d) * depthMeters / fx_d);
        double yCoordinate = ((i - cy_d) * depthMeters / fy_d);
        double zCoordinate = depthMeters;
        pc->PointXYZ[point_index] = static_cast<float>(xCoordinate);
        pc->Normals[point_index] = 0.0;
        pc->PointRGB[point_index] = colorVals[(3*i*columns) + (3*j) + 2];
        ++point_index;

        pc->PointXYZ[point_index] = static_cast<float>(yCoordinate);
        pc->Normals[point_index] = 0.0;
        pc->PointRGB[point_index] = colorVals[(3*i*columns) + (3*j) + 1];
        ++point_index;

        pc->PointXYZ[point_index] = static_cast<float>(zCoordinate);
        pc->Normals[point_index] = 0.0;
        pc->PointRGB[point_index] = colorVals[(3*i*columns) + (3*j) + 0];
        ++point_index;
        }
      }
    }
  return pc;
}
void Cloudy::UpdatePointCloud() {
  alert("FIXME: update");
}

void Cloudy::CreatePointCloud() {
  this->rgbImage = freenect_sync_get_rgb_cv(0);
  if (this->rgbImage)
    cvCvtColor(this->rgbImage, this->rgbImage, CV_RGB2BGR);
  else
    {
    this->m_isGood = false;
    return;
    //alert("Error: Kinect not connected?");
    }
  this->depthImage = freenect_sync_get_depth_cv(0);
  PointCloud * newcloud =
    depth_image_to_point_cloud(
      this->rgbImage, this->depthImage, this->MaximimDepth());

  if (newcloud != NULL)
    {
      if (this->pcloud != NULL)
        delete this->pcloud ; // FIXME best use of memeory?  How often
                              // does this get called?
      this->pcloud = newcloud;
    }
  this->m_isGood = true;
}
void Cloudy::ClearPointCloud() {
  delete this->pcloud;
  this->pcloud = NULL;
}


