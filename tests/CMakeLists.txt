find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})

set(TEST_FILES elf_utils_tests.cpp encryption_tests.cpp file_tests.cpp)

add_executable(crypter_tests main.cpp ${TEST_FILES})

target_link_libraries(crypter_tests crypter_lib ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
