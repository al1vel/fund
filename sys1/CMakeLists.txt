add_compile_options(-Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak)
add_link_options(-fsanitize=address -fsanitize=leak)

add_executable(sys1-1 lab1.c)
target_link_libraries(sys1-1 PRIVATE crypt)

add_executable(sys1-2 lab2.c)