cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_STANDARD 11)

project(ncnn-squeezenet)

# can set to command -DNCNN_SIMPLEOCV=ON
set(NCNN_SIMPLEOCV TRUE)

if(NOT NCNN_SIMPLEOCV)
    find_package(OpenCV QUIET COMPONENTS opencv_world)
    # for opencv 2.4 on ubuntu 16.04, there is no opencv_world but OpenCV_FOUND will be TRUE
    if("${OpenCV_LIBS}" STREQUAL "")
        set(OpenCV_FOUND FALSE)
    endif()
    if(NOT OpenCV_FOUND)
        find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio)
    endif()
    if(NOT OpenCV_FOUND)
        find_package(OpenCV QUIET COMPONENTS core highgui imgproc)
    endif()
endif()

if(OpenCV_FOUND OR NCNN_SIMPLEOCV)

    if(OpenCV_FOUND)
        message(STATUS "OpenCV library: ${OpenCV_INSTALL_PATH}")
        message(STATUS "    version: ${OpenCV_VERSION}")
        message(STATUS "    libraries: ${OpenCV_LIBS}")
        message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

        if(${OpenCV_VERSION_MAJOR} GREATER 3)
            set(CMAKE_CXX_STANDARD 11)
        endif()
    endif()

    #ncnn_add_example(squeezenet)
    
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../ncnn/src)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../../ncnn/src)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../ncnn/examples/src)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../../ncnn/examples/src)
    include_directories(./)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../ncnn ncnn_build)


    message(STATUS "SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})

    add_executable(squeezenet-main squeezenet.cpp)
    # if(OpenCV_FOUND)
    #     target_include_directories(${name} PRIVATE ${OpenCV_INCLUDE_DIRS})
    #     target_link_libraries(${name} PRIVATE ncnn ${OpenCV_LIBS})
    # elseif(NCNN_SIMPLEOCV)
    #     target_compile_definitions(${name} PUBLIC USE_NCNN_SIMPLEOCV)
    #     target_link_libraries(${name} PRIVATE ncnn)
    # endif()
    target_compile_definitions(squeezenet-main PUBLIC USE_NCNN_SIMPLEOCV)
    target_link_libraries(squeezenet-main PRIVATE ncnn)

    # add test to a virtual project group
    #set_property(TARGET ${name} PROPERTY FOLDER "examples")

endif()