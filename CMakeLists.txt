cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_C_COMPILER "gcc-5")
set(CMAKE_CXX_COMPILER "g++-5")
set(CMAKE_PREFIX_PATH "/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/inference/libs/libtorch")
project( inference )

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# include
include_directories( inference/include inference/src src inference/include/nlohmann)

# target
add_executable( inference inference/src/main.cpp inference/src/inference.cpp inference/src/turtle_shared_memory.cpp
        inference/src/simple_pg_model.cpp src/sl_shared_memory.c src/shared_memory.c inference/src/timer.cpp
        inference/src/timer.h inference/src/main.cpp inference/src/info_shared_memory.cpp
        inference/src/info_shared_memory.h inference/src/turtle.cpp inference/src/turtle.h
        inference/src/config_loader.cpp inference/src/config_loader.h inference/include/nlohmann/json.hpp
        inference/src/vpg_model.cpp inference/src/base_model.cpp inference/src/uniform_sampling_model.cpp)
target_link_libraries(inference "${TORCH_LIBRARIES}")
set_property(TARGET inference PROPERTY CXX_STANDARD 14)

