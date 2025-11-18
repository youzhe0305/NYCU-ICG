add_executable(ICG_2025_HW2
"main.cpp"
"stb_image.cpp"
) #列所有的cpp

target_link_libraries(ICG_2025_HW2
glfw
glm::glm
glad
tinyobjloader
)
