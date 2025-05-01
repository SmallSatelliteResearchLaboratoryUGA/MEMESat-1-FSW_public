set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/EpsUart.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/EpsUart.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/EpsUart.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/EpsUartTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/EpsUartTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/EpsUartTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
