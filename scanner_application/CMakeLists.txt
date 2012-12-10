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
#
# YOU MUST set the VTK_DIR, OpenCV_DIR, LIB_FREENECT_SRC, and
# LIB_FREENECT_BUILD variables.
#
# build openkinect/LibFreenect with
# cmake \
#     -DBUILD_CV:BOOL="1" \
#     -DOpenCV_DIR:PATH="...path-to.../build/OpenCV"

cmake_minimum_required(VERSION 2.6)

project( hephaestusscan )

set( VTK_DIR            "VTK_DIR-NOTFOUND" CACHE PATH "location of VTK libraries" )
set( OpenCV_DIR         "OpenCV_DIR-NOTFOUND" CACHE PATH "location of OpenCV libraries" )
set( LIBFREENECT_ROOT   "LIBFREENECT_ROOT-NOTFOUND" CACHE PATH "location of libfreenect root directory" )
set( LIBFREENECT_BUILD "LIBFREENECT_BUILD-NOTFOUND" CACHE PATH "location of libfreenect libraries" )

set(CMAKE_BUILD_TYPE debug)

if( ${CMAKE_COMPILER_IS_GNUCXX} )
  set(PCL_HEADER_COMPILE_OPTIONS "-Wno-unknown-pragmas -Wno-deprecated-declarations")
  set(CMAKE_CXX_FLAGS "-g -Wall ${PCL_HEADER_COMPILE_OPTIONS}")
endif( ${CMAKE_COMPILER_IS_GNUCXX} )
if( ${MSVC} )
	if(NOT PTHREADS_INCLUDE_DIR)
		set(PTHREADS_INCLUDE_DIR "PTHREADS_INCLUDE_DIR-NOTFOUND" CACHE PATH "location of pThreads include directory")
	endif()
	if(NOT PTHREADS_LIB)
		set(PTHREADS_LIB "PTHREADS_LIB-NOTFOUND" CACHE FILEPATH "location of the correct pThreadLib")
	endif()
endif( ${MSVC} )

## PCL ##
find_package(PCL)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

## VTK ##
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(QT_USE_OPENGL TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtScript REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

## OpenCV ##
find_package(OpenCV REQUIRED)

include_directories(
  "${LIBFREENECT_ROOT}/include/"
  "${LIBFREENECT_ROOT}/wrappers/opencv/"
  "${LIBFREENECT_ROOT}/wrappers/c_sync/" )

link_directories(
  "${LIBFREENECT_BUILD}/lib"
  ${OpenCV_LIB_DIR_DBG}
  ${OpenCV_LIB_DIR_OPT}
)

if( ${MSVC} )
set(SRC
  main.cxx
  CameraWidget.cxx
  Cloudy.cxx
  DepthScannerMainWindow.cxx
  QVTKPolyViewWidget.cxx
  PickFolder.cxx
  LoginDialog.cxx
  MidasUpload.cxx
  Parameters.cxx
  libfreenect_cv.c
  libfreenect_cv.h
)
else( ${MSVC} )
set(SRC
  main.cxx
  CameraWidget.cxx
  Cloudy.cxx
  DepthScannerMainWindow.cxx
  QVTKPolyViewWidget.cxx
  PickFolder.cxx
  LoginDialog.cxx
  MidasUpload.cxx
  Parameters.cxx
)
endif( ${MSVC} )

set(MOC
  CameraWidget.h
  DepthScannerMainWindow.h
  PickFolder.h
  LoginDialog.h
  Parameters.h
  Cloudy.h
)

qt4_wrap_cpp(MOC_GENERATED ${MOC})

add_executable(
  hephaestusscan
  ${SRC}
  ${MOC_GENERATED}
)

if( ${MSVC} )	
	target_link_libraries( hephaestusscan ${PTHREADS_LIB})
	include_directories(${PTHREADS_INCLUDE_DIR})
endif( ${MSVC} )

target_link_libraries(
  hephaestusscan
  ${VTK_LIBRARIES}
  ${OpenCV_LIBS}
  ${PCL_COMMON_LIBRARIES}
  ${PCL_IO_LIBRARIES}
  ${PCL_KDTREE_LIBRARY}
  ${PCL_SEARCH_LIBRARY}
  ${PCL_SURFACE_LIBRARY}
  ${PCL_FEATURES_LIBRARY}
  ${PCL_FILTERS_LIBRARY}
)

if(${MSVC})
target_link_libraries(
  hephaestusscan
  freenect
  freenect_sync
  ${QT_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_MAIN_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  QVTK
)
else(${MSVC})
target_link_libraries(
  hephaestusscan
  freenect_cv
  QVTK
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
)
endif(${MSVC})

