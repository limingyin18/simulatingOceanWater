file(GLOB_RECURSE SRC "*.hpp" "*.cpp")

add_library(basic ${SRC})

target_link_libraries(basic PUBLIC Eigen3::Eigen imgui::imgui glfw glad::glad)