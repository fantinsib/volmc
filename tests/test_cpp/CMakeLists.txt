####################################################
#       TESTS CMAKE
####################################################


add_executable(volmc_tests
test_models
test_schemes/tests_euler
test_engine/test_montecarlo

)

target_link_libraries(volmc_tests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE quadratic_exponential
)

target_include_directories(volmc_tests
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME volmc_tests
    COMMAND volmc_tests
)
