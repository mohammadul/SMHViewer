cmake_minimum_required(VERSION 3.10)

project(SMHViewer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_path(GLSTEREO_INCLUDE_DIR glstereo.h)
find_library(GLSTEREO_RELEASE_LIB glstereo)
find_path(MESHLIB_INCLUDE_DIR meshlib.h)
find_library(MESHLIB_RELEASE_LIB meshlib)

include_directories(mvparser PUBLIC ${GLSTEREO_INCLUDE_DIR} mvparser/include)
add_library(mvparser OBJECT mvparser/src/cfgpreter.cpp)

include_directories(mvutils PUBLIC ${GLSTEREO_INCLUDE_DIR} mvparser/include mvutils/include ${PNG_INCLUDE_DIRS})
add_library(mvutils OBJECT mvutils/src/merror.cpp mvutils/src/colours.cpp mvutils/src/tinypng.cpp)
target_link_libraries(mvutils PUBLIC ${PNG_LIBRARIES})

add_compile_definitions()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native -O2")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp -mwindows")

include_directories(modelviewer PUBLIC mvparser/include mvutils/include ${GLSTEREO_INCLUDE_DIR} ${MESHLIB_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
add_executable(modelviewer modelviewer/src/mvcanvas.cpp modelviewer/src/main.cpp modelviewer/resources/resource.rc)
if(WIN32)
target_link_libraries(modelviewer mvparser mvutils ${GLSTEREO_RELEASE_LIB} ${MESHLIB_RELEASE_LIB} png z freeglut_static SDL2_ttf SDL2main SDL2 glu32 opengl32 freetype version kernel32 ole32 oleaut32 winmm gdi32 wbemuuid)
else(WIN32)
target_link_libraries(modelviewer mvparser mvutils ${GLSTEREO_RELEASE_LIB} ${MESHLIB_RELEASE_LIB} png z freeglut_static SDL2_ttf SDL2main SDL2 GLu GL freetype)
endif(WIN32)

include_directories(multiviewer PUBLIC mvparser/include mvutils/include ${GLSTEREO_INCLUDE_DIR} ${MESHLIB_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
add_executable(multiviewer multiviewer/src/mvcanvas.cpp multiviewer/src/main.cpp multiviewer/resources/resource.rc)
if(WIN32)
target_link_libraries(multiviewer mvparser mvutils ${GLSTEREO_RELEASE_LIB} ${MESHLIB_RELEASE_LIB} png z freeglut_static SDL2_ttf SDL2main SDL2 glu32 opengl32 freetype version kernel32 ole32 oleaut32 winmm gdi32 wbemuuid)
else(WIN32)
target_link_libraries(multiviewer mvparser mvutils ${GLSTEREO_RELEASE_LIB} ${MESHLIB_RELEASE_LIB} png z freeglut_static SDL2_ttf SDL2main SDL2 GLu GL freetype)
endif(WIN32)

option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    get_filename_component(DOT_EXE_PATH ${DOXYGEN_DOT_EXECUTABLE} DIRECTORY)
    get_filename_component(DOT_EXE_PATH ${DOT_EXE_PATH} REALPATH)
    message(${DOT_EXE_PATH})
    
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
	add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen not found")
endif (DOXYGEN_FOUND)


