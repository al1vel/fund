add_compile_options(-Wall -Werror -Wextra -pedantic -fsanitize=address -fsanitize=leak)
add_link_options(-fsanitize=address -fsanitize=leak)
add_executable(logger logger.cpp)