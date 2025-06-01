file(GLOB SRC ${PROJECT_SOURCE_DIR}/example/callee/*.cpp ${PROJECT_SOURCE_DIR}/example/*.cc)
add_executable(mainee ${SRC})
target_link_libraries(mainee mprpc protobuf muduo_net muduo_base pthread)