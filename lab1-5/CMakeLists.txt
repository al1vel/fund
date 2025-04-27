cmake_minimum_required(VERSION 3.10)
#project(MyProject LANGUAGES CXX)

include(CTest)
enable_testing()

# Флаги компиляции
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Флаги для покрытия кода (активны только в Debug)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak)
    add_link_options(--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak)
endif()

# Добавляем GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем библиотеку
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
add_library(vector_lib ${SRC_FILES})
target_include_directories(vector_lib PUBLIC include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(vector_lib PRIVATE asan)
endif()

# Создаём отдельный исполняемый файл для тестов
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(tests_vector ${TEST_FILES})
target_link_libraries(tests_vector PRIVATE vector_lib GTest::gtest_main)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(tests_vector PRIVATE asan)
endif()

# Регистрируем тесты
add_test(NAME VectorTests COMMAND tests_vector)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Добавляем цель для покрытия кода
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    if(LCOV AND GENHTML)
        add_custom_target(vector_coverage
                COMMAND ${LCOV} --capture --directory . --ignore-errors mismatch --output-file coverage.info
                COMMAND ${LCOV} --remove coverage.info /usr/* */googletest/* */tests/* --output-file coverage_filtered.info
                COMMAND ${GENHTML} coverage_filtered.info --output-directory coverage_report
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Генерация отчёта покрытия кода..."
                VERBATIM
        )
    else()
        message(WARNING "lcov или genhtml не найдены, цель 'coverage' недоступна.")
    endif()
endif()

# Добавляем цель для анализа кода cppcheck
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    add_custom_target(vector_cppcheck
            COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet --suppress=missingIncludeSystem -I include src test
            COMMENT "Запуск cppcheck..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )
else()
    message(WARNING "cppcheck не найден, цель 'cppcheck' недоступна.")
endif()

add_custom_target(vector-clean-build
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*.a
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/test*
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage_filtered.info
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage.info
        COMMENT "Очистка собранных исполняемых файлов"
)

add_custom_target(vector-purge
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}
        COMMENT "Полная очистка всех артефактов сборки"
)

add_executable(vector_main src/main.cpp)
target_link_libraries(vector_main PRIVATE vector_lib)