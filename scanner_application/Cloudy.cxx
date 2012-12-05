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
#include "Parameters.h"

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

#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;

#define print(a) std::cerr << (#a) << ": " << (a) << '\n';

using namespace std;
using namespace cv;

typedef pcl::PointCloud<pcl::PointXYZRGBNormal> pcl_PointCloud;

static const short DEFAULT_MAXIMIM_DEPTH = 2047;
static const short DEFAULT_MINIMUM_DEPTH = 10;

// This struct serves as a private implementation object.
struct cloudy_configuration {
  std::vector<double> lookup_table;
  short maximimDepth;
  double fx_rgb, fy_rgb, cx_rgb, cy_rgb;
  double k1, k2, k3;
  double Rkx, Rky, Rkz, Tk;
  double Rix, Riy, Riz, Ti;
  double Rjx, Rjy, Rjz, Tj;
  double cx_d, cy_d, invfx_d, invfy_d;
};

/******************************************************************************/

static void alert(const char * s)
{
  if (QCoreApplication::instance() != NULL)
    QMessageBox::about(NULL, "Hephaestus", s);
  else
    std::cerr << s << '\n';
}

static void build_lookup_table(cloudy_configuration * config)
{
  std::vector<double> & table = config->lookup_table;
  double k1 = config->k1;
  double k2 = 1.0 / (config->k2);
  double k3 = config->k3;
  const int N = 2048;
  table.clear();
  table.reserve(N);
  for (int i = 0; i < N; ++i)
    table[i] = k3 * std::tan((k2 * static_cast<double>(i)) + k1);
}

static short meters_to_depth(double meters, cloudy_configuration * config)
{
  double k1 = - (config->k1);
  double k2 = (config->k2);
  double k3 = 1.0 / (config->k3);
  short s = 1 + static_cast<short>( (std::atan( meters * k3 ) + k1) * k2);
  if (s > DEFAULT_MAXIMIM_DEPTH)
    s = DEFAULT_MAXIMIM_DEPTH;
  else if (s < DEFAULT_MINIMUM_DEPTH)
    s = DEFAULT_MINIMUM_DEPTH;
  return s;
}

static inline void ijdepth_to_xyz_meters(
  int i, int j, short depth, double * xyz,
  cloudy_configuration * config)
{
  double & cx_d = config->cx_d;
  double & cy_d = config->cy_d;
  double & invfx_d = config->invfx_d;
  double & invfy_d = config->invfy_d;
  xyz[2] = config->lookup_table[static_cast<size_t>(depth)];
  xyz[0] = ((j - cx_d) * xyz[2] * invfx_d);
  xyz[1] = ((i - cy_d) * xyz[2] * invfy_d);
  return;
}

static inline int intround(double d)
{
  return static_cast<int>(std::floor(d + 0.5));
}

static inline void project_xyz_to_colorimg(double * xyz, int * colori, int *colorj, cloudy_configuration * config)
{
  double & fx_rgb = config->fx_rgb;
  double & fy_rgb = config->fy_rgb;
  double & cx_rgb = config->cx_rgb;
  double & cy_rgb = config->cy_rgb;

  double & Rkx = config->Rkx;
  double & Rky = config->Rky;
  double & Rkz = config->Rkz;
  double & Tk =  config->Tk ;

  double & Rix = config->Rix;
  double & Riy = config->Riy;
  double & Riz = config->Riz;
  double & Ti =  config->Ti ;

  double & Rjx = config->Rjx;
  double & Rjy = config->Rjy;
  double & Rjz = config->Rjz;
  double & Tj =  config->Tj ;

  double invZ = 1.0 / ( Rkx* xyz[0] + Rky * xyz[1] + Rkz* xyz[2] + Tk  );
  double x = ((Rix * xyz[0] + Riy* xyz[1] + Riz * xyz[2] + Ti) * fx_rgb * invZ) + cx_rgb;
  double y = ((Rjx * xyz[0] + Rjy* xyz[1] + Rjz * xyz[2] + Tj) * fy_rgb * invZ) + cy_rgb;
  *colori = intround(y);
  *colorj = intround(x);
  if (*colori < 0)
    *colori = 0;
  else if (*colori > 639)
    *colori = 639;
  if (*colorj < 0)
    *colorj = 0;
  else if (*colorj > 479)
    *colorj = 479;
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
  cloudy_configuration * config)
{
  if (rgbImage == NULL || depthImage == NULL)
    return NULL;
	assert(config != NULL);
  short infinity = config->maximimDepth;
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

  print(number_of_points);

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
        ijdepth_to_xyz_meters(i, j, depth, xyz, config);
        point.x = static_cast<float>(xyz[0]);
        point.y = static_cast<float>(xyz[1]);
        point.z = static_cast<float>(xyz[2]);
        int colori=i, colorj=j;
        project_xyz_to_colorimg(xyz, &colori, &colorj, config);
        point.r = colorVals[(((colori * columns) + colorj) * 3) + 0];
        point.g = colorVals[(((colori * columns) + colorj) * 3) + 1];
        point.b = colorVals[(((colori * columns) + colorj) * 3) + 2];
        pc->push_back(point);
        }
      }
    }
  return pc;
}

static pcl::PointCloud<pcl::PointXYZRGBNormal> * freenect_sync(
  cloudy_configuration * config, int freenect_index = 0)
{
  _IplImage * rgbImage;
  _IplImage * depthImage;
  rgbImage = freenect_sync_get_rgb_cv(freenect_index);
  if (rgbImage == NULL)
    return NULL;
  depthImage = freenect_sync_get_depth_cv(freenect_index);
  if (depthImage == NULL)
    return NULL;
  return depth_image_to_point_cloud(rgbImage, depthImage, config);
}

/******************************************************************************/
Cloudy::Cloudy(Parameters * parameters):
  m_isGood(false),
  pointCloud(NULL),
  parameters(parameters),
  config(new cloudy_configuration)
{
  assert (parameters != NULL);
  parameters->setDefault("Infinity in meters", "3.0");
  parameters->setDefault("fx_rgb", "5.471508098293293e+02");  // config->fx_rgb
  parameters->setDefault("fy_rgb", "5.3556393630057437e+02"); // config->fy_rgb
  parameters->setDefault("cx_rgb", "3.306272028759258e+02");  // config->cx_rgb
  parameters->setDefault("cy_rgb", "2.6748068171871557e+02"); // config->cy_rgb

  parameters->setDefault("depth_constant_k1", "1.1863");      // config->k1
  parameters->setDefault("depth_constant_k2", "2842.5");      // config->k2
  parameters->setDefault("depth_constant_k3", "0.1236");      // config->k3

  parameters->setDefault("Rkx", "1.7470421412464927e-02");
  parameters->setDefault("Rky", "1.2275341476520762e-02");
  parameters->setDefault("Rkz", "9.9977202419716948e-01");
  parameters->setDefault("Tk",  "-1.0916736334336222e-02f");

  parameters->setDefault("Rix", "9.9984628826577793e-01");
  parameters->setDefault("Riy", "1.2635359098409581e-03");
  parameters->setDefault("Riz", "-1.7487233004436643e-02");
  parameters->setDefault("Ti",  "1.9985242312092553e-02");

  parameters->setDefault("Rjx", "-1.4779096108364480e-03");
  parameters->setDefault("Rjy", "9.9992385683542895e-01");
  parameters->setDefault("Rjz", "-1.2251380107679535e-02");
  parameters->setDefault("Tj", "-7.4423738761617583e-04");

  parameters->setDefault("cx_d", "339.30780975300314");
  parameters->setDefault("cy_d", "242.73913761751615");
  parameters->setDefault("fx_d", "594.21434211923247");
  parameters->setDefault("fy_d", "591.04053696870778");
  QObject::connect(parameters, SIGNAL(changed()), this, SLOT(parametersChanged()));
  this->parametersChanged(); // get parameters from disc.
}

void Cloudy::parametersChanged()
{
  cloudy_configuration * config = this->config;

  config->fx_rgb = this->parameters->getParameter("fx_rgb").toDouble();
  config->fy_rgb = this->parameters->getParameter("fy_rgb").toDouble();
  config->cx_rgb = this->parameters->getParameter("cx_rgb").toDouble();
  config->cy_rgb = this->parameters->getParameter("cy_rgb").toDouble();

  config->k1 = this->parameters->getParameter("depth_constant_k1").toDouble();
  config->k2 = this->parameters->getParameter("depth_constant_k2").toDouble();
  config->k3 = this->parameters->getParameter("depth_constant_k3").toDouble();

  config->Rkx = this->parameters->getParameter("Rkx").toDouble();
  config->Rky = this->parameters->getParameter("Rky").toDouble();
  config->Rkz = this->parameters->getParameter("Rkz").toDouble();
  config->Tk  = this->parameters->getParameter("Tk ").toDouble();

  config->Rix = this->parameters->getParameter("Rix").toDouble();
  config->Riy = this->parameters->getParameter("Riy").toDouble();
  config->Riz = this->parameters->getParameter("Riz").toDouble();
  config->Ti  = this->parameters->getParameter("Ti").toDouble();

  config->Rjx = this->parameters->getParameter("Rjx").toDouble();
  config->Rjy = this->parameters->getParameter("Rjy").toDouble();
  config->Rjz = this->parameters->getParameter("Rjz").toDouble();
  config->Tj  = this->parameters->getParameter("Tj").toDouble();

  config->cx_d = this->parameters->getParameter("cx_d").toDouble();
  config->cy_d = this->parameters->getParameter("cy_d").toDouble();
  config->invfx_d = 1.0 / this->parameters->getParameter("fx_d").toDouble();
  config->invfy_d = 1.0 / this->parameters->getParameter("fy_d").toDouble();

	// must do this AFTER config->[PARAMETRS] are set.
  build_lookup_table(config);

	// must do this AFTER  build_lookup_table().
  config->maximimDepth = meters_to_depth(
    parameters->getParameter("Infinity in meters").toDouble(), config);
}

Cloudy::~Cloudy()
{
  delete this->config;
  delete this->pointCloud;
}

bool Cloudy::isGood()
{
  return this->m_isGood;
}

void Cloudy::GetCurrentPointCloud(vtkPolyData * output)
{
  point_cloud_to_vtkPolyData(this->pointCloud, output);
}



void Cloudy::UpdatePointCloud()
{
  if (this->pointCloud == NULL)
    {
    this->CreatePointCloud();
    return;
    }
  alert("FIXME: update");
  pcl_PointCloud * pc = freenect_sync(this->config, 0);
  if (pc == NULL)
    {
    this->m_isGood = false;
    return;
    }
  // FIXME!!!! do something to combine(pc and this->pointCloud)
}

void Cloudy::CreatePointCloud() {
  pcl_PointCloud * pc = freenect_sync(this->config, 0);
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

  print(cloud_size);

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
      xyz[1] = point.y;
      xyz[2] = point.z;
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
