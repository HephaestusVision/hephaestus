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
//#include "PointCloud.h"


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

#include <vtkCellArray.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>

using namespace std;
using namespace cv;

typedef pcl::PointCloud<pcl::PointXYZRGBNormal> pcl_PointCloud;

static void alert(const char * s)
{
  if (QCoreApplication::instance() != NULL)
    QMessageBox::about(NULL, "Hephaestus", s);
  else
    std::cerr << s << '\n';
}

static const short DEFAULT_MAXIMIM_DEPTH = 2047;

Cloudy::Cloudy(/* FIXME: constructor arguments? */):
  m_isGood(false),
  m_MaximimDepth(DEFAULT_MAXIMIM_DEPTH),
  pointCloud(NULL)
{
}

Cloudy::~Cloudy()
{
  delete this->pointCloud;
}

short Cloudy::MaximimDepth()
{
  return this->m_MaximimDepth;
}
void Cloudy::setMaximimDepth(short v)
{
  this->m_MaximimDepth = v;
}

bool Cloudy::isGood()
{
  return this->m_isGood;
}

void Cloudy::GetCurrentPointCloud(vtkPolyData * output)
{
  point_cloud_to_vtkPolyData(this->pointCloud, output);
}


static pcl::PointCloud<pcl::PointXYZRGBNormal> *  freenect_sync(
  int freenect_index = 0, short maximimDepth = DEFAULT_MAXIMIM_DEPTH)
{
  _IplImage * rgbImage;
  _IplImage * depthImage;
  rgbImage = freenect_sync_get_rgb_cv(freenect_index);
  if (rgbImage == NULL)
    return NULL;
  depthImage = freenect_sync_get_depth_cv(freenect_index);
  if (depthImage == NULL)
    return NULL;
  return depth_image_to_point_cloud(
    rgbImage, depthImage, maximimDepth);
}

void Cloudy::UpdatePointCloud() {
  if (this->pointCloud == NULL)
    {
    this->CreatePointCloud();
    return;
    }
  alert("FIXME: update");
  pcl_PointCloud * pc = freenect_sync(0, this->MaximimDepth());
  if (pc == NULL)
    {
    this->m_isGood = false;
    return;
    }
  // FIXME!!!! do something to combine(pc and this->pointCloud)
}

void Cloudy::CreatePointCloud() {
  pcl_PointCloud * pc = freenect_sync(0, this->MaximimDepth());
  if (pc == NULL)
    {
    this->m_isGood = false;
    return;
    }
  delete this->pointCloud; // no-op if already NULL;
  this->pointCloud = pc;
  this->m_isGood = true;
}

void Cloudy::ClearPointCloud() {
  delete this->pointCloud; // no-op if already NULL;
  this->pointCloud = NULL;
}


/******************************************************************************/

static inline void ijdepth_to_xyz_meters(int i, int j, short depth, double * xyz)
{
  static const double scaleFactor = 0.1; //100?
  static const double tmpVal1 = -0.0030711016;
  static const double tmpVal2 = 3.3309495161;
  static const double fx_d = (1.0 / 594.21434211923247);
  static const double fy_d = (1.0 / 591.04053696870778);
  static const double cx_d = 339.30780975300314;
  static const double cy_d = 242.73913761751615;
  double depthVal = static_cast<double>(depth);
  xyz[2] = scaleFactor / (tmpVal1 * depthVal + tmpVal2);
  xyz[0] = ((j - cx_d) * xyz[2] * fx_d);
  xyz[1] = ((i - cy_d) * xyz[2] * fy_d);
  return;
}

/**
   Given two IplImages representing RGB and depth information, create
   pcl::PointCloud<pcl::PointXYZRGBNormal> object.

   The returned pointer will point to a object created on the stack.
   it is the caller's repsonsibility to delete the object.

   Returns NULL on an error.
 */
pcl::PointCloud<pcl::PointXYZRGBNormal> * depth_image_to_point_cloud(
  _IplImage const * rgbImage,
  _IplImage const * depthImage,
  short infinity)
{
  if (rgbImage == NULL || depthImage == NULL)
    return NULL;
  int rows = depthImage->height;
  int columns = depthImage->width;
  int number_of_pixels = rows * columns;
  short *depthValues = reinterpret_cast<short*>(depthImage->imageData);
  unsigned char * colorVals = 
    reinterpret_cast<unsigned char*>(rgbImage->imageData);
  int number_of_points = 0;

  for (int i = 0; i < number_of_pixels; ++i)
    if ((depthValues[i]) < infinity)  
        ++number_of_points;

  pcl_PointCloud *pc =  new pcl_PointCloud;

  pc->reserve(number_of_points);

  pcl::PointXYZRGBNormal point;
  point.a = 0xff; // opaque
  point.normal[0] = 0.0; // unknown
  point.normal[1] = 0.0;
  point.normal[2] = 0.0;

  for (int i = 0; i < rows; ++i)
    {
    for (int j = 0; j < columns; ++j)
      {
      double depth = depthValues[(i * columns) + j];
      if (depth < infinity)
        {
        double xyz[3];
        ijdepth_to_xyz_meters(i, j, depth, xyz);
        point.x = static_cast<float>(xyz[0]);
        point.y = static_cast<float>(xyz[1]);
        point.z = static_cast<float>(xyz[2]);
        // point.r = colorVals[(3*i*columns) + (3*j) + 2];
        // point.g = colorVals[(3*i*columns) + (3*j) + 1];
        // point.b = colorVals[(3*i*columns) + (3*j) + 0];
        point.r = colorVals[(((i * columns) + j) * 3) + 0];
        point.g = colorVals[(((i * columns) + j) * 3) + 1];
        point.b = colorVals[(((i * columns) + j) * 3) + 2];
        pc->push_back(point);
        }
      }
    }
  return pc;
}

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
  vtkPolyData * output)
{
  if (cloud == NULL)
    return 1;
  vtkSmartPointer< vtkPolyData > poly =
    vtkSmartPointer< vtkPolyData >::New(); // create new data object
  vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
  poly->SetPoints(pts);

  size_t cloud_size = cloud->size();
  if (cloud_size > 0)
    {
    pts->SetNumberOfPoints(cloud_size);
    vtkIdType numPoints = static_cast< vtkIdType >(cloud_size);

    vtkPointData * pd = poly->GetPointData();
    assert(pd != NULL);

    vtkSmartPointer<vtkUnsignedCharArray> colorArray =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorArray->SetName("rgb_colors");
    colorArray->SetNumberOfComponents(3);
    colorArray->SetNumberOfTuples(numPoints);

    vtkSmartPointer< vtkFloatArray > normalsArray =
      vtkSmartPointer< vtkFloatArray >::New();
    normalsArray->SetName("Normals");
    normalsArray->SetNumberOfComponents(3);
    normalsArray->SetNumberOfTuples(numPoints);

    vtkSmartPointer< vtkCellArray > verts =
      vtkSmartPointer< vtkCellArray >::New();
    verts->Allocate(2 * static_cast<int>(numPoints));

    for (vtkIdType ptId = 0; ptId < numPoints; ++ptId)
      {
      const pcl::PointXYZRGBNormal & point = cloud->at(static_cast<size_t>(ptId));
      float xyz[3];
      float normals[3];
      unsigned char rgb[3];
      xyz[0] = point.x;
      xyz[0] = point.y;
      xyz[0] = point.z;
      rgb[0] = point.r;
      rgb[1] = point.g;
      rgb[2] = point.b;
      pts->SetPoint(ptId, xyz);
      colorArray->SetTupleValue(ptId, rgb);
      normalsArray->SetTupleValue(ptId, normals);
      verts->InsertNextCell(1);
      verts->InsertCellPoint(ptId);
      }
    pd->SetScalars(colorArray);
    pd->AddArray(normalsArray);
    poly->SetVerts(verts);
    } // end_if (cloud->NumPoints > 0)
  output->ShallowCopy(poly);
  return 0;
}

/**
   Function to convert from vtkPolyData to a
   pcl::PointCloud<pcl::PointXYZRGBNormal>.  Will get color
   information from the rgb_colors array if present.

   The returned pointer will point to a object created on the stack.
   it is the caller's repsonsibility to delete the object.

   Returns NULL on an error.
  */
pcl::PointCloud<pcl::PointXYZRGBNormal> * vtkPolyData_to_point_cloud(
  vtkPolyData * poly)
{
  vtkIdType numberOfPoints = poly->GetNumberOfPoints();
  size_t number_of_points = static_cast<size_t>(numberOfPoints);

  if (number_of_points == 0)
    {
    return NULL; 
    }

  pcl_PointCloud *pc = new pcl_PointCloud;
  
  vtkPoints * pts = poly->GetPoints();
  vtkPointData * pd = poly->GetPointData();
  assert(pd != NULL);

  vtkUnsignedCharArray * colorArray =
    vtkUnsignedCharArray::SafeDownCast( 
      pd->GetArray("rgb_colors"));
  if ((colorArray != NULL) && (colorArray->GetNumberOfComponents() != 3))
    colorArray = NULL;

  vtkDataArray * normalsDataArray = pd->GetNormals();
  if ((normalsDataArray != NULL)
      && (normalsDataArray->GetNumberOfComponents() != 3))
    normalsDataArray = NULL; // must have 3 components.

  vtkFloatArray * normalsArray =
    (normalsDataArray != NULL) ? 
    vtkFloatArray::SafeDownCast(normalsDataArray) :
    NULL;

  pc->reserve(number_of_points);

  pcl::PointXYZRGBNormal point;
  point.a = 0xff; // opaque
  point.r = 0xff; // default value: white
  point.g = 0xff;
  point.b = 0xff;
  point.normal[0] = 0.0; // default value: no vector
  point.normal[1] = 0.0;
  point.normal[2] = 0.0;

  for (vtkIdType ptId = 0; ptId < numberOfPoints; ++ptId)
    {
    double xyz[3];
    pts->GetPoint(ptId, xyz);
    point.x = xyz[0];
    point.y = xyz[1];
    point.z = xyz[2];
    if (colorArray != NULL)
      {
      unsigned char rgb[3];
      colorArray->GetTupleValue(ptId, rgb);
      // already checked that colorArray->NumberOfComponents == 3
      point.r = rgb[0];
      point.g = rgb[1];
      point.b = rgb[2];
      }
    if (normalsArray != NULL)
      {
      float normals[3];
      normalsArray->GetTupleValue(ptId, normals);
      point.normal[0] = normals[0];
      point.normal[1] = normals[1];
      point.normal[2] = normals[2];
      }
    else if (normalsDataArray != NULL)
      {
      double normals_d[3];
      normalsDataArray->GetTuple(ptId, normals_d);
      point.normal[0] = normals_d[0];
      point.normal[1] = normals_d[1];
      point.normal[2] = normals_d[2];
      }
    pc->push_back(point);
    }
  return pc; 
}
