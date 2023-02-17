project(ubuntu-intro)

#set(NCNN_BUILD_BASE_PATH "/home/tuduweb/development/lightweight/ncnn/build-arm-linux-gnueabihf")
set(NCNN_BUILD_BASE_PATH "/home/tuduweb/development/lightweight/ncnn/build-aarch64-linux-gnu")
#/home/tuduweb/development/lightweight/ncnn/build-aarch64-linux-gnu
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

if(CMAKE_TOOLCHAIN_FILE)
    set(LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_BINARY_DIR} CACHE PATH "root for library output, set this to change where android libs are compiled to")
    # get absolute path, but get_filename_component ABSOLUTE only refer with source dir, so find_file here :(
    get_filename_component(CMAKE_TOOLCHAIN_FILE_NAME ${CMAKE_TOOLCHAIN_FILE} NAME)
    find_file(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE_NAME} PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
    message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
endif()


link_directories(${NCNN_BUILD_BASE_PATH}/install/lib/)
include_directories(${NCNN_BUILD_BASE_PATH}/install/include/ncnn)

# set_target_properties(${TARGET} PROPERTIES
#     COMPILE_FLAGS "-pthread"
#     LINK_FLAGS "-pthread"
#     )

add_executable(
    ${PROJECT_NAME}
    main.cpp
    )

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ncnn
    
    pthread
    )
