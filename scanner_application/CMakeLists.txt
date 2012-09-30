#   Hephaestus Scanner Application
#
#   Copyright 2012 University of North Carolina at Chapel Hill.
#
#   Licensed under the Apache License, Version 2.0 (the "License"); you
#   may not use this file except in compliance with the License.  You
#   may obtain a copy of the License at
#
#     LICENSE.txt in this repository or
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
#   implied.  See the License for the specific language governing
#   permissions and limitations under the License.

cmake_minimum_required(VERSION 2.6)

project( hephaestusscan )

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "-g -Wall")

#set the VTK_DIR variable
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(QT_USE_OPENGL TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(OpenCV REQUIRED)

## build openkinect/LibFreenect with
## cmake -DBUILD_CV:BOOL="1" -DOpenCV_DIR:PATH="/home/hal/build/OpenCV"

set(LIB_FREENECT_SRC "$ENV{LIB_FREENECT_SRC}")
set(LIB_FREENECT_BUILD "$ENV{LIB_FREENECT_BUILD}")

include_directories(
  "${LIB_FREENECT_SRC}/include"
  "${LIB_FREENECT_SRC}/wrappers/opencv/"
  "${LIB_FREENECT_SRC}/wrappers/c_sync/" )
link_directories(
  "${LIB_FREENECT_BUILD}/lib"
  ${OpenCV_LIB_DIR_DBG}
  ${OpenCV_LIB_DIR_OPT}
)

set(SRC
  main.cxx
  CameraWidget.cxx
  Cloudy.cxx
  DepthScannerMainWindow.cxx
  PointCloud.cxx
  QVTKPolyViewWidget.cxx
)

link_libraries("freenect_cv")

set(MOC
  CameraWidget.h
  DepthScannerMainWindow.h
)
qt4_wrap_cpp(MOC_GENERATED ${MOC})

SOURCE_GROUP("Generated" FILES ${MOCSrcs} ${RCS_SOURCES} )

add_executable(
  hephaestusscan
  ${SRC}
  ${MOC_GENERATED}
)

LINK_DIRECTORIES(
  ${QT_LIBRARIES}
  ${QT_LIBRARY_DIR}
  ${QT_QTOPENGL_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_MAIN_LIBRARY}
  )


target_link_libraries(
  hephaestusscan
  ${VTK_LIBRARIES}
  QVTK
  ${QT_QTOPENGL_LIBRARY}
  ${OpenCV_LIBS})


