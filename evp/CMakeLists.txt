cmake_minimum_required(VERSION 3.10)


add_library(evp-gui gui.cpp gui.hpp)
target_link_libraries(evp-gui PUBLIC sfml-graphics sfml-window sfml-system)
set_target_properties(evp-gui PROPERTIES
   CXX_STANDARD 11
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO
)



