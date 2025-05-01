set(SOURCE_FILES

    "${CMAKE_CURRENT_LIST_DIR}/EpsUartPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/StateMachinePorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/SchedulerPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileRecyclerPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/UpdaterPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/MessagePorts.fpp"

)

register_fprime_module()