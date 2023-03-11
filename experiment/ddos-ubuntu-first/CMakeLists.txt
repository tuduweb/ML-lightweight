project(ubuntu-intro)

link_directories(/home/tuduweb/development/lightweight/ncnn/build/install/lib/)
include_directories(/home/tuduweb/development/lightweight/ncnn/build/install/include/ncnn)

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
