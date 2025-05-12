add_compile_options(-Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak)
add_link_options(-fsanitize=address -fsanitize=leak)

add_executable(sys1-1 lab1.c)
target_link_libraries(sys1-1 PRIVATE crypt)

add_executable(sys1-2 lab2.c)
add_executable(sys1-3 lab3.c)
add_executable(sys1-5 lab5.c)
add_executable(sys1-6c lab6-client.c)
add_executable(sys1-6s lab6-server.c)
add_executable(sys1-7 lab7.c)