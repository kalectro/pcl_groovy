# Look for ROS.

# A macro for linking the ROS libraries.  (Might not be necessary... todo: add this logic at the find_package(ROS))
macro(link_ros_libs _target)
  target_link_libraries(${_target} ${catkin_LIBRARIES})
endmacro(link_ros_libs)

option(USE_ROS "Integrate with ROS rather than using native files" ON)
message(STATUS "Shall I look for ROS?; USE_ROS is ${USE_ROS}")
if(USE_ROS)
	# Search for ROS
	find_package(catkin REQUIRED COMPONENTS roscpp_serialization std_msgs sensor_msgs rostime)
	include_directories(${catkin_INCLUDE_DIRS})
	
	# use, i.e. don't skip the full RPATH for the build tree
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

	# the RPATH to be used when installing
	SET(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/../../lib64")
	# don't add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif(USE_ROS)

