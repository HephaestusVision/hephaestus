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

#include <pcl/registration/icp.h>
#include <pcl/filters/voxel_grid.h>

//// debugging code
#define print(a) std::cerr << (#a) << ": " << (a) << '\n';

using namespace Eigen;
using namespace std;

typedef pcl::PointCloud<pcl::PointXYZRGBNormal> pcl_PointCloud;

static const short DEFAULT_MAXIMIM_DEPTH = 2047;
static const short DEFAULT_MINIMUM_DEPTH = 10;
static const int IMAGE_WIDTH = 640;
static const int IMAGE_HEIGHT = 480;

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
	double videoLength;
	double imageBatchSize;
	double imageJump;
	vector<short> depthImages;
	vector<char> rgbImages;
	int currentImage;
	int maxImages;
	int numberOfImagesToJump;
	pcl::PointCloud<pcl::PointXYZRGBNormal> *currentPointCloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal> *previousPointCloud1;
	int previousImageNumber1;
	//pcl::PointCloud<pcl::PointXYZRGBNormal> *previousPointCloud2;
	//int previousImageNumber2;
};

/******************************************************************************/

// for errors.
static void alert(const char * s)
{
  if (QCoreApplication::instance() != NULL)
    QMessageBox::about(NULL, "Hephaestus", s);
  else
    std::cerr << s << '\n';
}

// since there are MANY more function calls than possible values, we
// speed things up by precomputing each of the 2048 possible values.
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

// the user sets a threshold in meters.  we want to convert to
// internal units to make comparisons faster.
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

// convert to pixel index units to world units.
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

// round to nearest int.
static inline int intround(double d)
{
  return static_cast<int>(std::floor(d + 0.5));
}

// Convert from xyz world coordinates relative to the depth camera to
// ij pixel coordinates in the color camera image.
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
   Given two pointers representing RGB and depth information, create
   pcl::PointCloud<pcl::PointXYZRGBNormal> object.

   The returned pointer will point to a object created on the stack.
   it is the caller's repsonsibility to delete the object.

   Returns NULL on an error.
 */
static pcl::PointCloud<pcl::PointXYZRGBNormal> * depth_image_to_point_cloud(
  short * depthValues, 
	char * colorValues,
  cloudy_configuration * config)
{
  if (colorValues == NULL || depthValues == NULL)
    return NULL;
  assert(config != NULL);
  short infinity = config->maximimDepth;
  int rows = IMAGE_HEIGHT;
  int columns = IMAGE_WIDTH;
  int number_of_pixels = rows * columns;
  int number_of_points = 0;

  for (int i = 0; i < number_of_pixels; ++i)
    if ((depthValues[i]) < infinity)
        ++number_of_points;

  pcl_PointCloud *pc =  new pcl_PointCloud;

  pc->reserve(number_of_points);

  //print(number_of_points);

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
        point.r = colorValues[(((colori * columns) + colorj) * 3) + 0];
        point.g = colorValues[(((colori * columns) + colorj) * 3) + 1];
        point.b = colorValues[(((colori * columns) + colorj) * 3) + 2];
        pc->push_back(point);
        }
      }
    }
  return pc;
}

/**
	Returns the starting index in rgbImages of the image numbered imageNumber
*/
static int get_index_rgb_image(int imageNumber)
{
	return(3*IMAGE_WIDTH*IMAGE_HEIGHT*imageNumber);
}

/**
	Returns the starting index in depthImages of the image numbered imageNumber
*/
static int get_index_depth_image(int imageNumber)
{
	return(IMAGE_WIDTH*IMAGE_HEIGHT*imageNumber);
}

/**
	Returns the point cloud associated with the next image returned by depth_image_to_point_cloud.
	Reutrns NULL if there are no more images in the sequence.
*/
static pcl::PointCloud<pcl::PointXYZRGBNormal> * get_next_point_cloud(cloudy_configuration * config)
{
	cout << "Getting Next Image" << endl;
	print(config->currentImage);
	if(config->currentImage==config->maxImages-1)
	{
		return(NULL);
	}
	else
	{
		config->currentImage = config->currentImage + 1;
		int rgbIndex = get_index_rgb_image(config->currentImage);
		int depthIndex = get_index_depth_image(config->currentImage);
		return(depth_image_to_point_cloud(&(config->depthImages[depthIndex]),&(config->rgbImages[rgbIndex]),config));
	}
}


// Talk to the Kinect, get rgb and depth images from the depth camera.
// Store those images into the memory provided and already allocated by the caller.
static bool freenect_sync(
  short * depthImageLocation, char * rgbImageLocation, int freenect_index = 0)
{
  _IplImage * rgbImage, * depthImage;
  rgbImage = freenect_sync_get_rgb_cv(freenect_index);
  if (rgbImage == NULL)
    return false;
  depthImage = freenect_sync_get_depth_cv(freenect_index);
  if (depthImage == NULL)
    return false;
	assert(depthImage->height == IMAGE_HEIGHT);
	assert(depthImage->width == IMAGE_WIDTH);
	memcpy(depthImageLocation,depthImage->imageData,IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(short));
	memcpy(rgbImageLocation,rgbImage->imageData,3*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(char));
	return true;
}

/******************************************************************************/
Cloudy::Cloudy(Parameters * parameters):
  m_isGood(false),
  pointCloud(NULL),
  parameters(parameters),
  config(new cloudy_configuration)
{
	this->config->currentPointCloud = NULL;
	this->config->previousPointCloud1 = NULL;

  assert (parameters != NULL);
  parameters->setDefault("Infinity in meters", "3.0");

	parameters->setDefault("Length of Creation Video in frames", 25);
	parameters->setDefault("Images Processed In One Update", 1);
	parameters->setDefault("Next Image Number = Old Image Number + This", 1);

  // RGB CAMERA
  //   Focal Length
  parameters->setDefault("RGB Focal Length X", 530.32033); // config->fx_rgb
  parameters->setDefault("RGB Focal Length Y", 531.84388); // config->fy_rgb
  //   Principal point
  parameters->setDefault("RGB Principal Point X", 312.97306);  // config->cx_rgb
  parameters->setDefault("RGB Principal Point Y", 265.00603); // config->cy_rgb

  // DEPTH CAMERA
  //   Focal Length
  parameters->setDefault("Depth Focal Length X", 585.97379); // config->fx_d
  parameters->setDefault("Depth Focal Length Y", 584.00903); // config->fy_d
  //   Principal point
  parameters->setDefault("Depth Principal Point X", 319.50000); // config->cx_d
  parameters->setDefault("Depth Principal Point Y", 239.50000); // config->cy_d

  parameters->setDefault("depth_constant_k1", 1.1863); // config->k1
  parameters->setDefault("depth_constant_k2", 2842.5); // config->k2
  parameters->setDefault("depth_constant_k3", 0.1236); // config->k3

  parameters->setDefault("Rix", 0.99984628826577793);
  parameters->setDefault("Riy", 0.0012635359098409581);
  parameters->setDefault("Riz", -0.017487233004436643);
  parameters->setDefault("Ti",  0.019985242312092553);

  parameters->setDefault("Rjx", -0.0014779096108364480);
  parameters->setDefault("Rjy", 0.99992385683542895);
  parameters->setDefault("Rjz", -0.012251380107679535);
  parameters->setDefault("Tj", -0.00074423738761617583);
	
  parameters->setDefault("Rkx", 0.017470421412464927);
  parameters->setDefault("Rky", 0.012275341476520762);
  parameters->setDefault("Rkz", 0.99977202419716948);
  parameters->setDefault("Tk",  -0.010916736334336222);

  QObject::connect(parameters, SIGNAL(changed()), this, SLOT(parametersChanged()));
  this->parametersChanged(); // get parameters from disc.
}

void Cloudy::parametersChanged()
{
  cloudy_configuration * config = this->config;

	config->videoLength = this->parameters->getValue("Length of Creation Video in frames");
	config->imageBatchSize = this->parameters->getValue("Images Processed In One Update");
	config->imageJump = this->parameters->getValue("Next Image Number = Old Image Number + This");

  config->fx_rgb = this->parameters->getValue("RGB Focal Length X");
  config->fy_rgb = this->parameters->getValue("RGB Focal Length Y");
  config->cx_rgb = this->parameters->getValue("RGB Principal Point X");
  config->cy_rgb = this->parameters->getValue("RGB Principal Point Y");

  config->k1 = this->parameters->getValue("depth_constant_k1");
  config->k2 = this->parameters->getValue("depth_constant_k2");
  config->k3 = this->parameters->getValue("depth_constant_k3");

  config->Rkx = this->parameters->getValue("Rkx");
  config->Rky = this->parameters->getValue("Rky");
  config->Rkz = this->parameters->getValue("Rkz");
  config->Tk  = this->parameters->getValue("Tk ");

  config->Rix = this->parameters->getValue("Rix");
  config->Riy = this->parameters->getValue("Riy");
  config->Riz = this->parameters->getValue("Riz");
  config->Ti  = this->parameters->getValue("Ti");

  config->Rjx = this->parameters->getValue("Rjx");
  config->Rjy = this->parameters->getValue("Rjy");
  config->Rjz = this->parameters->getValue("Rjz");
  config->Tj  = this->parameters->getValue("Tj");

  config->cx_d = this->parameters->getValue("Depth Principal Point X");
  config->cy_d = this->parameters->getValue("Depth Principal Point Y");
  config->invfx_d =
    1.0 / this->parameters->getValue("Depth Focal Length X");
  config->invfy_d =
    1.0 / this->parameters->getValue("Depth Focal Length Y");

  // must do this AFTER config->[PARAMETRS] are set.
  build_lookup_table(config);

  // must do this AFTER  build_lookup_table().
  config->maximimDepth = meters_to_depth(
    parameters->getValue("Infinity in meters"), config);
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
	point_cloud_to_vtkPolyData(this->config->currentPointCloud, output);
}

int Cloudy::UpdateSinglePointCloud(bool deleteOriginalPointCloud)
{
	pcl::PointCloud<pcl::PointXYZRGBNormal> *originalPointCloud = this->config->currentPointCloud;
	pcl::PointCloud<pcl::PointXYZRGBNormal> *newPointCloud = get_next_point_cloud(this->config);
	if(originalPointCloud==NULL)
	{
		alert("No Current Point Cloud");
		return(-2);
	}
	else if(newPointCloud==NULL)
	{
		alert("The End of the Sequence Has Been Reached");
		return(-1);
	}
	else
	{
		//Merge the point clouds
		pcl::IterativeClosestPoint<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal> icp;
		icp.setMaxCorrespondenceDistance(0.02);
		icp.setMaximumIterations(50);
		icp.setTransformationEpsilon (1e-9);
		icp.setEuclideanFitnessEpsilon (0.0000001);
		pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr originalPointCloudSharedPointer(originalPointCloud->makeShared());
		pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr newPointCloudSharedPointer(newPointCloud);
		icp.setInputCloud(originalPointCloudSharedPointer);
		icp.setInputTarget(newPointCloudSharedPointer);
		cout << "Beginning Combining Point Clouds Algorithm Execution!" << endl;
		pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr alignedPointCloudPtrSharedPointer(new pcl::PointCloud<pcl::PointXYZRGBNormal>); 

		icp.align(*(alignedPointCloudPtrSharedPointer.get()));
		cout << "has converged:" << icp.hasConverged() << " score: " <<	icp.getFitnessScore() << endl;

		pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr finalPointCloudFullResSharedPointer(new pcl::PointCloud<pcl::PointXYZRGBNormal>);

		cout << "Combining" << endl;
		*(finalPointCloudFullResSharedPointer.get()) = 
			*(alignedPointCloudPtrSharedPointer.get()) + 
			*(finalPointCloudFullResSharedPointer.get());

		cout << "DownSampling" << endl;
		const float final_voxel_grid_size = 0.003f;
		pcl::VoxelGrid<pcl::PointXYZRGBNormal> vox_grid;
		vox_grid.setInputCloud (finalPointCloudFullResSharedPointer);
		vox_grid.setLeafSize (final_voxel_grid_size, final_voxel_grid_size, final_voxel_grid_size);
		pcl::PointCloud<pcl::PointXYZRGBNormal> *finalPointCloudDownsampled = new pcl::PointCloud<pcl::PointXYZRGBNormal>;
		vox_grid.filter (*finalPointCloudDownsampled);
		cout << "Deleting" << endl;
		if(deleteOriginalPointCloud)
		{
			delete originalPointCloud;
		}
		//delete newPointCloud;
		//delete alignedPointCloud;
		//delete finalPointCloudFullRes;
		cout <<"Setting New Final" <<endl;
		this->config->currentPointCloud=finalPointCloudDownsampled;
		cout << "Completed Combining Point Clouds Algorithm Execution!" << endl;
		return(0);
	}
}

  /**
     Modify the current point cloud by adding another frame.
  */
void Cloudy::UpdatePointCloud()
{
	if(this->config->currentPointCloud==NULL)
	{
		alert("The Current Point Cloud is NULL!");
	}
	else
	{
		int numberToDo = static_cast<int>(this->config->imageBatchSize);
		if(numberToDo < 1)
		{
			numberToDo=1;
		}
		this->config->numberOfImagesToJump=static_cast<int>(this->config->imageJump);
		if(this->config->numberOfImagesToJump < 1)
		{
			this->config->numberOfImagesToJump=1;
		}
		int counter = 0;
		int updateResponse = 0;
		if(this->config->previousPointCloud1!=NULL)
		{
			cout << "Update: Deleting previousPointCloud" << endl;
			delete this->config->previousPointCloud1;
		}
		cout << "Update: Setting previousPointCloud" << endl;
		this->config->previousPointCloud1=this->config->currentPointCloud;
		this->config->previousImageNumber1=this->config->currentImage;
		cout << "Update: Now onto updating" << endl;
		while((counter < numberToDo) && (updateResponse==0))
		{
			this->config->currentImage = this->config->currentImage + this->config->numberOfImagesToJump - 1;
			print(counter);
			bool deleteCurrentPointCloud=true;
			if(counter==0)
			{
				cout << "Update: Not deleting current point cloud" << endl;
				deleteCurrentPointCloud=false;
			}
			updateResponse = UpdateSinglePointCloud(deleteCurrentPointCloud);
			counter = counter+1;
		}
		if(counter!=0)
		{
			cout << "Update: Checking Point Clouds" << endl;
			cout << "previousPointCloud has " << this->config->previousPointCloud1->points.size() << " points" << endl;
			cout << "currentPointCloud has " << this->config->currentPointCloud->points.size() << " points" << endl;
		}
	}
}

  /**
		 Take a N-second video.  The length of the video is determined by Parameters.
		 Produce first frame pointcloud
		 Any old PC will be lost.
  */
void Cloudy::CreatePointCloud() {
	//Create Images
	assert (this->config != NULL);
	this->config->maxImages = static_cast<int>(this->config->videoLength);
	if(this->config->maxImages<1)
	{
		this->config->maxImages=1;
	}
	this->config->currentImage=-1; //Since currentPointCloud has nothing in it, the currentImage being displayed it the -1st one, a non-valid image;
	this->config->depthImages.resize(IMAGE_WIDTH*IMAGE_HEIGHT*this->config->maxImages);
	this->config->rgbImages.resize(3*IMAGE_WIDTH*IMAGE_HEIGHT*this->config->maxImages);
	int i=0;
	bool success = true;
	while((success == true)&&(i<this->config->maxImages))
	{
		if((i%50)==0)
			print(i);
		short * currentDepthImgLocation = &(this->config->depthImages[get_index_depth_image(i)]);
		char * currentRgbImgLocation = &(this->config->rgbImages[get_index_rgb_image(i)]);
		success = freenect_sync(currentDepthImgLocation,currentRgbImgLocation, 0);
		i = i + 1;
	}
	if(success == false)
	{
		this->config->maxImages=0;
	}
	//Produce first frame pointcloud
	this->config->currentPointCloud = get_next_point_cloud(config);
	if(this->config->previousPointCloud1!=NULL)
	{
		delete this->config->previousPointCloud1;
	}
	this->config->previousPointCloud1=NULL;
	this->config->previousImageNumber1=-1;
	//this->config->previousPointCloud2=NULL;
	//this->config->previousImageNumber2=-1;
}

  /**
     Reset to initial condition.
  */
void Cloudy::ClearPointCloud() {
	//fixme
  delete this->pointCloud; // no-op if already NULL;
  this->pointCloud = NULL;
}

  /**
     If possible, revert to state before the last call to UpdatePointCloud().
  */
void Cloudy::RevertPointCloud()
{
	if(this->config->currentPointCloud==NULL)
	{
		alert("The Current Point Cloud is NULL!");
	}
	else
	{
		cout << "Reverting" << endl;
		if(0 < this->config->previousImageNumber1)
		{
			cout << "Deleting Old Current Point Cloud" << endl;
			delete this->config->currentPointCloud;
			cout << "Setting Up Current Image" << endl;
			this->config->currentImage=this->config->previousImageNumber1;
			this->config->currentPointCloud = this->config->previousPointCloud1;
			cout << "Resetting Previous Point Cloud" << endl;
			this->config->previousImageNumber1=-1;
			this->config->previousPointCloud1 = NULL;
		}
		else
		{
			cout << "Reset to the Beginning" << endl;
			this->config->currentImage=-1;
			this->config->currentPointCloud = get_next_point_cloud(config);
			this->config->previousImageNumber1=-1;
			this->config->previousPointCloud1 = NULL;
		}
	}
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

  //print(cloud_size);

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
