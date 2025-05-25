add_compile_options(-Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak)
add_link_options(-fsanitize=address -fsanitize=leak)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
add_library(lib ${SRC_FILES})
target_include_directories(lib PUBLIC include)

add_executable(sys3_client client.cpp)
target_link_libraries(sys3_client PRIVATE lib)

add_executable(sys3_server server.cpp)
target_link_libraries(sys3_server PRIVATE lib)