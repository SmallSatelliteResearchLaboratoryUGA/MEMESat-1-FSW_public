set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Scheduler.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Scheduler.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Scheduler.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SchedulerTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/SchedulerTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SchedulerTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()