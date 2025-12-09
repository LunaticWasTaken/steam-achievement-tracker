# sourcepp
set(SOURCEPP_LIBS_START_ENABLED OFF CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_KVPP         ON CACHE INTERNAL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sourcepp")

# nlohmann-json
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/nlohmann-json")

# yaml-cpp
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/yaml-cpp")

# spdlog
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/spdlog")

# lntc_cpp
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/lntc_cpp")