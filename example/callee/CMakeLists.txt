file(GLOB SRC ${PROJECT_SOURCE_DIR}/example/callee/*.cpp ${PROJECT_SOURCE_DIR}/example/caller/*.cc)
add_executable(main ${SRC})