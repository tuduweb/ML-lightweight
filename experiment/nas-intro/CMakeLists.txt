project(ubuntu-intro)

set(NCNN_BUILD_BASE_PATH "/home/tuduweb/development/lightweight/ncnn/build-debug")

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
    ncnnd #ncnnd
    
    pthread
    )
