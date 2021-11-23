aux_source_directory(. PLAYER_SRC_LIST)

include_directories(../third_party/ffmpeg/include)
include_directories(../third_party/sdl2/include)

LINK_DIRECTORIES(../third_party/ffmpeg/lib)
LINK_DIRECTORIES(../third_party/sdl2/lib)
LINK_DIRECTORIES(../third_party/h264)
LINK_DIRECTORIES(../third_party/h265)
LINK_DIRECTORIES(../third_party/fdk-aac)

add_executable(player ${PLAYER_SRC_LIST})
target_link_libraries(player ${CMAKE_PROJECT_NAME}_static  avformat avcodec avutil swscale z m SDL2)
