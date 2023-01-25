cmake_minimum_required(VERSION 3.18.4)
project(Rose2)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wconversion -Wno-attributes -Wno-unknown-pragmas)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" "${CMAKE_MODULE_PATH}")
INCLUDE(CheckCXXCompilerFlag)
FIND_FILE(BCMHOST bcm_host.h PATHS /opt/vc/include NO_DEFAULT_PATH)
IF(BCMHOST)
    add_compile_definitions(BCMHOST=1)
ELSE (BCMHOST)
    add_compile_definitions(X86HOST=1)
ENDIF (BCMHOST)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(SDL2TTF REQUIRED)
find_package(CURLPP REQUIRED)
find_package(Soci REQUIRED COMPONENTS sqlite3)
find_package(LocalTime REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2TTF_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${CURLPP_INCLUDE_DIR})
include_directories(${SOCI_INCLUDE_DIRS})
include_directories(${LOCALTIME_INCLUDE_DIR})

include_directories(include src/util fmt/include)

list(APPEND LIB_FMT fmt/src/format.cc fmt/src/os.cc)

list(APPEND EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 sqlite3 pthread dl rt m stdc++fs uuid)

list(APPEND RoseLibraries Rose2 ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${CURLPP_LIBRARIES} ${SOCI_LIBRARY}
        ${SOCI_sqlite3_PLUGIN} ${LOCALTIME_LIBRARY} ${EXTRA_LIBS})

list(APPEND ROSE_SRC src/Application.cpp src/GraphicsModel.cpp src/Surface.cpp src/Rose.cpp src/Gadget.cpp src/Color.cpp
        src/Window.cpp src/Font.cpp src/TextGadget.cpp src/RowColumn.cpp src/Event.cpp src/Theme.cpp src/Border.cpp
        src/Singlet.cpp src/Widget.cpp src/TimerTick.cpp src/TextSet.cpp src/Material.cpp src/Animation.cpp src/Button.cpp)

add_library(Rose2 SHARED ${ROSE_SRC} ${LIB_FMT})

add_executable(Test test/test.cpp)
target_link_libraries(Test ${RoseLibraries})
