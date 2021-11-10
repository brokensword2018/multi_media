aux_source_directory(. TEST_SRC_LIST)
foreach(TEST_SRC ${TEST_SRC_LIST})
    string(REGEX REPLACE "^\\./|\\.cpp$" "" TEST_EXE_NAME ${TEST_SRC})
    message(STATUS "添加测试程序:${TEST_EXE_NAME}")

    add_executable(${TEST_EXE_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_EXE_NAME}  ${CMAKE_PROJECT_NAME}_static)
endforeach(TEST_SRC ${TEST_SRC_LIST})

