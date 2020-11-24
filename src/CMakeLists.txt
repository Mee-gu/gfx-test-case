# add -lite external here for test project

# include modules
if(WIN32)
    set(CMAKE_MODULE_PATH ${GFX_EXTERNAL_PATH}/cmake)
    include(CocosBuildSet)
endif()


set(USE_SE_V8 ON)
list(REMOVE_ITEM CC_EXTERNAL_LIBS "glfw3")

# resources
set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(VS)
    cocos_mark_multi_resources(cc_common_res RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()
set(APP_UI_RES
    "${GAME_RES_FOLDER}/uv_checker_01.jpg"
    "${GAME_RES_FOLDER}/uv_checker_02.jpg"
    "${GAME_RES_FOLDER}/stencil.jpg"
    "${GAME_RES_FOLDER}/background.png"
    "${GAME_RES_FOLDER}/sprite0.png"
)

# add test cases
include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)

if(WIN32)
    add_definitions("-DCC_STATIC")
    add_subdirectory(win)
elseif(IOS)
	add_subdirectory(ios)
elseif(ANDROID)
    add_subdirectory(android/app/src/main/cpp)
elseif(APPLE)
    add_definitions("-DUSE_METAL")
    add_subdirectory(mac)

    # add_definitions("-DUSE_GLES2")
    # add_subdirectory(mac-gles)
endif()
