set(SOURCE_FILES

    "${CMAKE_CURRENT_LIST_DIR}/OTAUpdater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/OTAUpdater.cpp"
)

set(MOD_DEPS
    Components/FileRecycler
    Components/StateMachine
    Components/Scheduler
)

register_fprime_module()

set(UT_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/OTAUpdater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OTAUpdaterTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/OTAUpdaterTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/OTAUpdaterTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
