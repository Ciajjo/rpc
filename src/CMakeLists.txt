file(GLOB RPC ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.cc)
add_library(mprpc SHARED ${RPC})
