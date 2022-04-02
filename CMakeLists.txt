cmake_minimum_required(VERSION 3.10...3.21)

project(mythtv-controller
  VERSION
    1.0
  DESCRIPTION
    "Very nice project"
  LANGUAGES
<<<<<<< ours
    C CXX
)
# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#add_library(json-parser STATIC json-parser/json.c json-parser/json.c)
add_executable(mythtv-controller main.cpp)
=======
    CXX
)
add_executable(mythtv-controller main.cpp)


>>>>>>> theirs
