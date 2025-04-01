set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/EpsUartTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/StateMachineTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SchedulerTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileRecyclerTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/UpdaterTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/MessageTypes.fpp"
)

register_fprime_module()