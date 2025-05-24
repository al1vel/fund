#add_compile_options(-Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak)
#add_link_options(-fsanitize=address -fsanitize=leak)

add_executable(sys3_server_example server_example.cpp)
add_executable(sys3_client client.cpp)
add_executable(sys3_compile_subserver compile_subserver.cpp)
add_executable(sys3_game_subserver game_subserver.cpp)
add_executable(sys3_server server.cpp)
