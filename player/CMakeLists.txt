aux_source_directory(. PLAYER_SRC_LIST)


add_executable(player ${PLAYER_SRC_LIST})
target_link_libraries(player ${CMAKE_PROJECT_NAME}_static  avformat avcodec avutil swscale z m SDL2)
