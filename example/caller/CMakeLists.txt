file(GLOB SRC ${PROJECT_SOURCE_DIR}/example/caller/*.cpp ${PROJECT_SOURCE_DIR}/example/*.cc)
add_executable(mainer ${SRC})
target_link_libraries(mainer mprpc protobuf muduo_net muduo_base pthread zookeeper_mt)