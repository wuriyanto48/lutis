cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 14)

project (lutis)
include_directories(${CMAKE_JS_INC})

set(HEADER_FILES
    plugin/Type.h
    plugin/Draw.h
    plugin/Filter.h
    plugin/Core.h
)

set(SOURCE_FILES 
    plugin/Lutis.cc
)

MESSAGE( [Main] "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

# -------------------- WebP --------------------

# set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/vendor/webp/lib/pkgconfig")
# set(WebP_DIR ${PROJECT_SOURCE_DIR}/vendor/webp/libwebp-1.2.1)
# set(WebP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/webp/include/webp)
# set(WebP_LIBRARIES ${PROJECT_SOURCE_DIR}/vendor/webp/lib)

# find_package(WebP CONFIG REQUIRED)

# -------------------- Image Magick --------------------
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/vendor/image_magick/lib/pkgconfig")
set(ImageMagick_EXECUTABLE_DIR ${PROJECT_SOURCE_DIR}/vendor/image_magick/bin)
set(ImageMagick_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/image_magick/include/ImageMagick-7)
set(ImageMagick_LIBRARIES ${PROJECT_SOURCE_DIR}/vendor/image_magick/lib)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(ImageMagick REQUIRED COMPONENTS convert Magick++ MagickCore)

# find_program(MAGICK_CONFIG "${ImageMagick_EXECUTABLE_DIR}/Magick++-config")

# # Ask about CXX and lib flags/locations
# execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
# execute_process(COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
# # Remove trailing whitespace (CMAKE warns about this)
# string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
# string(STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)
# # Append all flags to CMAKE_CXX_FLAGS
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${MAGICK_CXX_FLAGS} ${MAGICK_LD_FLAGS}")

# -------------------- OPEN CV --------------------

# set OpenCV_DIR variable equal to the path to the cmake
# files within the previously installed opencv program
set(OpenCV_DIR ${PROJECT_SOURCE_DIR}/vendor/opencv/install/lib/cmake/opencv4)

# Tell compiler to use C++ 14 features which is needed because
# Clang version is often behind in the XCode installation
set(CMAKE_CXX_STANDARD 14)

# configure the necessary common CMake environment variables
# needed to include and link the OpenCV program into this
# demo project, namely OpenCV_INCLUDE_DIRS and OpenCV_LIBS
find_package( OpenCV REQUIRED )

# tell the build to include the headers from OpenCV
# include_directories( ${OpenCV_INCLUDE_DIRS}  ${ImageMagick_INCLUDE_DIRS} ${WebP_INCLUDE_DIRS})
include_directories( ${OpenCV_INCLUDE_DIRS}  ${ImageMagick_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${OpenCV_LIBS} ${ImageMagick_LIBRARIES} ${WebP_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${OpenCV_LIBS} ${ImageMagick_LIBRARIES})

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

MESSAGE( [Main] " PROJECT_NAME = ${PROJECT_NAME}")
MESSAGE( [Main] " CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
MESSAGE( [Main] " CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
MESSAGE( [Main] " PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE( [Main] " ImageMagick_FOUND = ${ImageMagick_FOUND}")
MESSAGE( [Main] " ImageMagick_INCLUDE_DIRS = ${ImageMagick_INCLUDE_DIRS}")
MESSAGE( [Main] " ImageMagick_LIBRARIES = ${ImageMagick_LIBRARIES}")
MESSAGE( [Main] " OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
MESSAGE( [Main] " MAGICK_CONFIG = ${MAGICK_CONFIG}")
MESSAGE( [Main] " WEBP_FOUND = ${WEBP_FOUND}")

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=4)