add_subdirectory(basic)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Scene.cpp Ocean.cpp OceanRender.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE basic)

add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders  $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
  )