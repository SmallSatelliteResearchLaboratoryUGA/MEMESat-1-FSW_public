set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/StateMachine.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/StateMachine.cpp"
)

set(MOD_DEPS
    Components/EpsUart
)

register_fprime_module()

set(UT_SOURCE_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/StateMachine.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StateMachineTester.cpp"
    # "${CMAKE_CURRENT_LIST_DIR}/test/ut/StateMachineTesterHelpers.cpp" # need to comment out to build. will tend to later. 
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StateMachineTestMain.cpp"

)

set(UT_MOD_DEPS
    STest
)

set(UT_AUTO_HELPERS ON) 
register_fprime_ut()