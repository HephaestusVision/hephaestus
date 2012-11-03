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

# YOU MUST set the VTK_DIR, OpenCV_DIR, LIB_FREENECT_SRC, and
# LIB_FREENECT_BUILD variables.

cmake_minimum_required(VERSION 2.6)

project( hephaestusscan )

set( VTK_DIR            "" CACHE PATH "location of VTK libraries" )
set( OpenCV_DIR         "" CACHE PATH "location of OpenCV libraries" )
set( LIB_FREENECT_SRC   "" CACHE PATH "location of libfreenext source" )
set( LIB_FREENECT_BUILD "" CACHE PATH "location of libfreenext libraries" )

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "-g -Wall")

find_package(PCL 1.3 REQUIRED COMPONENTS common io search)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(QT_USE_OPENGL TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtScript REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(OpenCV REQUIRED)

## build openkinect/LibFreenect with
## cmake -DBUILD_CV:BOOL="1" -DOpenCV_DIR:PATH="...path-to.../build/OpenCV"
include_directories(
  "${LIB_FREENECT_SRC}/include/"
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
  PickFolder.cxx
  LoginDialog.cxx
  MidasUpload.cxx
)

link_libraries("freenect_cv")

set(MOC
  CameraWidget.h
  DepthScannerMainWindow.h
  PickFolder.h
  LoginDialog.h
)
qt4_wrap_cpp(MOC_GENERATED ${MOC})

SOURCE_GROUP("Generated" FILES ${MOCSrcs} ${RCS_SOURCES} )

add_executable(
  hephaestusscan
  ${SRC}
  ${MOC_GENERATED}
)

# message( STATUS  ${QT_LIBRARIES} )
# message( STATUS  ${QT_LIBRARY_DIR} )
# message( STATUS  ${QT_QTOPENGL_LIBRARY} )
# message( STATUS  ${QT_QTGUI_LIBRARY} )
# message( STATUS  ${QT_QTCORE_LIBRARY} )
# message( STATUS  ${QT_MAIN_LIBRARY} )
#  ${QT_LIBRARIES}

# LINK_DIRECTORIES(
#   ${QT_LIBRARY_DIR}
#   ${QT_QTOPENGL_LIBRARY}
#   ${QT_QTGUI_LIBRARY}
#   ${QT_QTCORE_LIBRARY}
#   ${QT_MAIN_LIBRARY}
#   )

target_link_libraries(
  hephaestusscan
  ${VTK_LIBRARIES}
  QVTK
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${OpenCV_LIBS}
  ${PCL_COMMON_LIBRARIES}
  ${PCL_IO_LIBRARIES}
  ${PCL_KDTREE_LIBRARY}
  ${PCL_SEARCH_LIBRARY}
  ${PCL_SURFACE_LIBRARY}
  ${PCL_FEATURES_LIBRARY} )
