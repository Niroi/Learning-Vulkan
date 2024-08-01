cmake_minimum_required(VERSION 3.8)
project(vulkantest)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw
    ${GLFW_LIBRARIES} 
    Vulkan::Vulkan 
    ${COMMON_LINKER_FLAGS}
)

target_compile_options(${PROJECT_NAME} PUBLIC ${COMMON_COMPILER_FLAGS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "vulkantest")
