set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/TlmChanWrapper.fpp"    
    "${CMAKE_CURRENT_LIST_DIR}/TlmChanWrapper.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/TlmChanWrapper.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanWrapperTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanWrapperTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanWrapperTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
