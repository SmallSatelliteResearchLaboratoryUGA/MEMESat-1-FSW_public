set(SOURCE_FILES 

    "${CMAKE_CURRENT_LIST_DIR}/FileRecycler.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileRecycler.cpp"
) 

set(MOD_DEPS
   Svc/FileDownlink
   Components/Scheduler
   Components/StateMachine
)

register_fprime_module() 

set(UT_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/FileRecycler.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileRecyclerTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileRecyclerTesterHelpers.cpp" # need to comment out to build. will tend to later.
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FileRecyclerTestMain.cpp"
)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()