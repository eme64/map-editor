cmake_minimum_required(VERSION 3.10)

project(map-editor)
add_executable(map-editor main.cpp)
target_link_libraries(map-editor PUBLIC evp-gui)
target_link_libraries(map-editor PUBLIC sfml-graphics sfml-window sfml-system)
set_target_properties(map-editor PROPERTIES
   CXX_STANDARD 11
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO
)


