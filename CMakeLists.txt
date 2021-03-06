# Copyright (C) 2007-2013 Istituto Italiano di Tecnologia ADVR & iCub Facility
# Authors: Enrico Mingo, Alessio Rocchi, Mirko Ferrati, Silvio Traversaro and Alessandro Settimi
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

PROJECT(GazeboYarpPlugins)

find_package(YARP 2.3.63.2 REQUIRED)
message(STATUS "YARP is version: ${YARP_VERSION}")

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Finding dependencies
find_package(Gazebo REQUIRED)

find_package(Boost REQUIRED serialization system)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
  pkg_check_modules(SDFORMAT sdformat)
  pkg_check_modules(PROTOBUF protobuf)
endif()

# add local cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Define (depending on CMAKE_BUILD_TYPE) the DEBUG macro
include(AddDebugMacro)

# Enable all possible warning if CMAKE_BUILD_TYPE is debug
include(AddDebugWarnings)

include_directories(include/gazebo_yarp_plugins)
include_directories(SYSTEM ${YARP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${GAZEBO_INCLUDE_DIRS}  ${SDFORMAT_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS} ${SDFORMAT_LIBRARY_DIRS} ${PROTOBUF_LIBRARY_DIRS})

set(commons_headers         include/gazebo_yarp_plugins/common.h)
#if this is not a target it will not work :(
SOURCE_GROUP("Common" FILES ${commons_headers})
# SET_SOURCE_FILES_PROPERTIES(${commons_headers} PROPERTIES HEADER_FILE_ONLY TRUE)

set(singleton_source        src/gazebo_plugins/Handler.cc)
set(singleton_headers       include/gazebo_yarp_plugins/Handler.hh)

SOURCE_GROUP("Source Files" FILES ${singleton_source})
SOURCE_GROUP("Header Files" FILES ${singleton_headers})

add_library(gazebo_yarp_singleton    SHARED ${singleton_source}    ${singleton_headers})
target_link_libraries(gazebo_yarp_singleton ${YARP_LIBRARIES} ${SDFORMAT_LIBRARIES} ${GAZEBO_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS gazebo_yarp_singleton
                DESTINATION lib)
                
set(controlBoard_source     src/gazebo_plugins/ControlBoard.cc
                            src/yarp_drivers/ControlBoardDriver.cpp
                            src/yarp_drivers/ControlBoardDriverControlMode.cpp
                            src/yarp_drivers/ControlBoardDriverInteractionMode.cpp
                            src/yarp_drivers/ControlBoardDriverDeviceDriver.cpp
                            src/yarp_drivers/ControlBoardDriverEncoders.cpp
                            src/yarp_drivers/ControlBoardDriverOthers.cpp
                            src/yarp_drivers/ControlBoardDriverPositionControl.cpp
                            src/yarp_drivers/ControlBoardTorqueDriverControl.cpp
                            src/yarp_drivers/ControlBoardDriverVelocityControl.cpp
                            src/yarp_drivers/ControlBoardDriverImpedancePositionControl.cpp
                            src/yarp_drivers/ControlBoardDriverOpenLoop.cpp
                            src/yarp_drivers/ControlBoardDriverPidControl.cpp)

set(controlBoard_headers    include/gazebo_yarp_plugins/ControlBoard.hh
                            include/gazebo_yarp_plugins/ControlBoardDriver.h )

SOURCE_GROUP("Source Files" FILES ${controlBoard_source})
SOURCE_GROUP("Header Files" FILES ${controlBoard_headers})

add_library(gazebo_yarp_controlboard SHARED ${controlBoard_source} ${controlBoard_headers})
target_link_libraries(gazebo_yarp_controlboard  gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})
# Add install target
install(TARGETS gazebo_yarp_controlboard
                DESTINATION lib)

set(forceTorque_source      src/gazebo_plugins/ForceTorque.cc
                            src/yarp_drivers/ForceTorqueDriver.cpp)

set(forceTorque_headers     include/gazebo_yarp_plugins/ForceTorque.hh
                            include/gazebo_yarp_plugins/ForceTorqueDriver.h)


SOURCE_GROUP("Source Files" FILES ${forceTorque_source})
SOURCE_GROUP("Header Files" FILES ${forceTorque_headers})

add_library(gazebo_yarp_forcetorque  SHARED ${forceTorque_source}  ${forceTorque_headers})
target_link_libraries(gazebo_yarp_forcetorque   gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ForceTorquePlugin  ${Boost_LIBRARIES})

# Add install target
install(TARGETS gazebo_yarp_forcetorque
                DESTINATION lib)

set(imu_headers             include/gazebo_yarp_plugins/IMU.hh
                            include/gazebo_yarp_plugins/IMUDriver.h)
set(imu_source              src/gazebo_plugins/IMU.cc
                            src/yarp_drivers/IMUDriver.cpp)

SOURCE_GROUP("Source Files" FILES ${imu_source})
SOURCE_GROUP("Header Files" FILES ${imu_headers})

add_library(gazebo_yarp_imu SHARED ${imu_source} ${imu_headers})
target_link_libraries(gazebo_yarp_imu gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

# Add install target
install(TARGETS gazebo_yarp_imu
                DESTINATION lib)

set(jointSensors_headers    include/gazebo_yarp_plugins/JointSensors.hh
                            include/gazebo_yarp_plugins/JointSensorsDriver.h)
set(jointSensors_source     src/gazebo_plugins/JointSensors.cc
                            src/yarp_drivers/JointSensorsDriver.cpp)

SOURCE_GROUP("Source Files" FILES ${jointSensors_source})
SOURCE_GROUP("Header Files" FILES ${jointSensors_headers})

add_library(gazebo_yarp_jointsensors SHARED ${jointSensors_source} ${jointSensors_headers})
target_link_libraries(gazebo_yarp_jointsensors  gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})
# Add install target
install(TARGETS gazebo_yarp_jointsensors
                DESTINATION lib)

set(clock_headers           include/gazebo_yarp_plugins/Clock.hh include/gazebo_yarp_plugins/ClockServerImpl.h)
set(clock_source            src/gazebo_plugins/Clock.cc src/gazebo_plugins/ClockServerImpl.cpp)

yarp_idl_to_dir(thrift/clock_rpc.thrift ${CMAKE_CURRENT_SOURCE_DIR}/autogenerated CLOCKRPC_SRC CLOCKRPC_HEADERS CLOCKRPC_INCLUDES)

SOURCE_GROUP("Source Files" FILES ${clock_source})
SOURCE_GROUP("Header Files" FILES ${clock_headers})
SOURCE_GROUP("Source Files\\Autogenerated" FILES ${CLOCKRPC_SRC})
SOURCE_GROUP("Header Files\\Autogenerated" FILES ${CLOCKRPC_HEADERS})

include_directories(${CLOCKRPC_INCLUDES})
add_library(gazebo_yarp_clock SHARED ${clock_source} ${clock_headers} ${CLOCKRPC_SRC} ${CLOCKRPC_HEADERS})
target_link_libraries(gazebo_yarp_clock ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

# Add install target
install(TARGETS gazebo_yarp_clock
                DESTINATION lib)

set(externalwrench_source   src/gazebo_plugins/ApplyExternalWrench.cc)
set(externalwrench_headers  include/gazebo_yarp_plugins/ApplyExternalWrench.hh)

SOURCE_GROUP("Source Files" FILES ${externalwrench_source})
SOURCE_GROUP("Header Files" FILES ${externalwrench_headers})

add_library(gazebo_yarp_externalwrench   SHARED ${externalwrench_source}   ${externalwrench_headers})
target_link_libraries(gazebo_yarp_externalwrench gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

# Add install target
install(TARGETS gazebo_yarp_externalwrench
                DESTINATION lib)

#set(timeSynchronizer_source  src/gazebo_plugins/TimeSynchronizer.cc)
#set(timeSynchronizer_headers include/gazebo_yarp_plugins/TimeSynchronizer.hh)

#SOURCE_GROUP("Source Files" FILES ${timeSynchronizer_source})
#SOURCE_GROUP("Header Files" FILES ${timeSynchronizer_headers})

set(camera_headers          include/gazebo_yarp_plugins/Camera.hh
                            include/gazebo_yarp_plugins/CameraDriver.h)
set(camera_source           src/gazebo_plugins/Camera.cc
                            src/yarp_drivers/CameraDriver.cpp)
SOURCE_GROUP("Source Files" FILES ${camera_source})
SOURCE_GROUP("Header Files" FILES ${camera_headers})

add_library(gazebo_yarp_camera SHARED ${camera_source}       ${camera_headers})
target_link_libraries(gazebo_yarp_camera gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} CameraPlugin)

# Add install target
install(TARGETS gazebo_yarp_camera
                DESTINATION lib)

set(showmodelcom_source   src/gazebo_plugins/ShowModelCoM.cc)
set(showmodelcom_headers  include/gazebo_yarp_plugins/ShowModelCoM.hh)

SOURCE_GROUP("Source Files" FILES ${showmodelcom_source})
SOURCE_GROUP("Header Files" FILES ${showmodelcom_headers})

add_library(gazebo_yarp_showmodelcom SHARED ${showmodelcom_source}       ${showmodelcom_headers})
target_link_libraries(gazebo_yarp_showmodelcom gazebo_yarp_singleton ${YARP_LIBRARIES} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

# Add install target
install(TARGETS gazebo_yarp_showmodelcom
                DESTINATION lib)

# Add uninstall target
include(AddUninstallTarget)
