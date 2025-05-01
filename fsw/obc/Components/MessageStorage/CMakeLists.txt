set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/MessageStorage.fpp"    
    "${CMAKE_CURRENT_LIST_DIR}/MessageStorage.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/MessageStorage.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/MessageStorageTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/MessageStorageTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/MessageStorageTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()