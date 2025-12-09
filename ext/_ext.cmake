# sourcepp
set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sourcepp")

# nlohmann-json
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/json")

# lntc_cpp
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/lntc_cpp")

# spdlog
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/spdlog")

# yaml-cpp
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/yaml-cpp")
