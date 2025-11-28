cmake_minimum_required(VERSION 3.22)
project(sonia_deploy)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#==========================================================================
# F I N D  D E P E N D E N C I E S
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sonia_common_ros2 REQUIRED)
find_package(std_srvs REQUIRED)
find_package(std_msgs REQUIRED)

#==========================================================================
# I N C L U D E / S O U R C E S  D I R E C T O R I E S
include_directories(include)
file(GLOB SOURCES src/*.cpp)

#==========================================================================
# C R E A T E   E X E C U T A B L E
add_executable(${PROJECT_NAME} ${SOURCES})

#==========================================================================
# I N S T A L L   D I R E C T O R I E  S
install(
  DIRECTORY include/
  DESTINATION include
)
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

#==========================================================================
# L I N K  D E P E N D E N C I E S
ament_target_dependencies(${PROJECT_NAME} rclcpp sonia_common_ros2 std_msgs std_srvs)

ament_package()
