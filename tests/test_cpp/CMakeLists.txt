####################################################
#       TESTS CMAKE
####################################################


add_executable(quadratic_exponential_tests
test_models
test_schemes/tests_euler
test_engine/test_montecarlo

)

target_link_libraries(quadratic_exponential_tests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE quadratic_exponential
)

target_include_directories(quadratic_exponential_tests
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME quadratic_exponential_tests
    COMMAND quadratic_exponential_tests
)
