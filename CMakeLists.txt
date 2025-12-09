
cmake_minimum_required(VERSION 4.0)
project(steam-achievement-tracker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(find_package_git)
  include(FetchContent)
  if (${ARGC} EQUAL 3)
    FetchContent_Declare(${ARGV0} GIT_REPOSITORY ${ARGV1} GIT_TAG ${ARGV2})
    FetchContent_MakeAvailable(${ARGV0})
  elseif (${ARGC} EQUAL 2)
    FetchContent_Declare(${ARGV0} GIT_REPOSITORY ${ARGV1} GIT_TAG master)
    FetchContent_MakeAvailable(${ARGV0})
  endif ()
endmacro()

macro(cs_include_directory)
  # See https://github.com/craftablescience/cmake-helpers/blob/main/CS_IncludeDirectory.cmake
  set(__CS_DETAIL_DIR "${ARGV0}")
  cmake_path(GET __CS_DETAIL_DIR FILENAME __CS_DETAIL_DIR_FILENAME)
  include("${CMAKE_CURRENT_SOURCE_DIR}/${__CS_DETAIL_DIR}/_${__CS_DETAIL_DIR_FILENAME}.cmake")
endmacro()

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

set(QT_VERSION "6.10.1")
set(QT_ARCH "msvc2022_64")
set(QT_LIBDIR "${PROJECT_BINARY_DIR}/qtlib")
file(MAKE_DIRECTORY "${QT_LIBDIR}")
if (NOT EXISTS "${QT_LIBDIR}/install/${QT_VERSION}/${QT_ARCH}")
  message("QT not found!")
  if (NOT EXISTS "${QT_LIBDIR}/aqt.exe")
    # AQT is an alternate open-source qt installer that doesn't require you to sell your soul. See https://github.com/miurahr/aqtinstall
    message("Downloading aqt...")
    execute_process(COMMAND "powershell" "-Command" "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://github.com/miurahr/aqtinstall/releases/download/v3.3.0/aqt.exe' -OutFile '${QT_LIBDIR}/aqt.exe'")
  endif ()
  message("Downloading QT v${QT_VERSION}...")
  execute_process(COMMAND "${QT_LIBDIR}/aqt.exe" "install-qt" "windows" "desktop" "${QT_VERSION}" "win64_${QT_ARCH}" "--outputdir" "${QT_LIBDIR}/install" "--archives" "qtbase")
  message("Done.")
else ()
  message("QT Found!")
endif ()
set(QT_DIR "${QT_LIBDIR}/install/${QT_VERSION}/${QT_ARCH}")
set(Qt6_DIR "${QT_DIR}/lib/cmake/Qt6")

cs_include_directory(ext)

# find_package(nlohmann_json CONFIG REQUIRED)
# find_package(spdlog CONFIG REQUIRED)
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
# find_package_git(lntc_cpp https://github.com/LunaticWasTaken/lntc_cpp master)

target_link_libraries("${PROJECT_NAME}" PRIVATE
    nlohmann_json::nlohmann_json
    yaml-cpp::yaml-cpp
    spdlog::spdlog
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    lntc_cpp
    sourcepp::kvpp
)

target_precompile_headers("${PROJECT_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/pch.hpp")
target_include_directories("${PROJECT_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/include")

# For running the app inside the ide
add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${QT_DIR}/bin/Qt6Core.dll" "${PROJECT_BINARY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${QT_DIR}/bin/Qt6Gui.dll" "${PROJECT_BINARY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${QT_DIR}/bin/Qt6Widgets.dll" "${PROJECT_BINARY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/plugins/platforms/"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/plugins/imageformats/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${QT_DIR}/plugins/platforms/qwindows.dll" "${PROJECT_BINARY_DIR}/plugins/platforms/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${QT_DIR}/plugins/imageformats/qjpeg.dll" "${PROJECT_BINARY_DIR}/plugins/imageformats/"
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${PROJECT_BINARY_DIR}/resources"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/resources" "${PROJECT_BINARY_DIR}/resources"
)

# Create a release
set(DEPLOY_DIR "${PROJECT_BINARY_DIR}/release-win")
add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/LICENSE.md" "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/third-party-licenses" "${DEPLOY_DIR}/third-party-licenses"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/release-scripts" "${DEPLOY_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/resources" "${DEPLOY_DIR}/resources"
)
