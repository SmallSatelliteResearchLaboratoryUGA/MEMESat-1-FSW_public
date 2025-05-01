module EpsModule { 

    active component EpsUart { 

        # --- 
        # Serial Ports 
        # ---

        @ Input port for receiving UART data 
        async input port UartRead: Drv.ByteStreamRecv

        @ Input port for components to send commands to the EPS
        async input port ExternCommandBuffer: EpsModule.EPS_CommandRecvPort

        @ Output port for sending commands to the StateMachine
        output port SM_CommandBuffer: EpsModule.EPS_CommandRecvPort

        @ Output Port for writing UART data
        output port UartWrite: Drv.ByteStreamSend

        @ Output port for sending UART buffers to use for reading
        output port UartBuffers: Fw.BufferSend

        @ Output port for changing the state of the system 
        output port ChangeState: EpsModule.EPS_ChangeState

        @ Output port for requesting the state of the system
        output port RequestState: EpsModule.RequestState

        # --- 
        # Special ports 
        # --- 

        @ Command receive
        command recv port cmdIn 

        @ Command registration
        command reg port cmdRegOut

        @ Command response
        command resp port cmdResponseOut

        @ Event port
        event port eventOut

        @ Parameter get 
        param get port prmGetOut

        @ Parameter set
        param set port prmSetOut

        @ Telemetry
        telemetry port tlmOut

        @ Text event
        text event port logTextOut

        @ Time get
        time get port timeCaller

        # --- 
        # Commands 
        # --- 

        @ Command to report the status of the EPS
        async command EPS_ReportStatus

        @ Command to tell eps to listen to the Radio 
        async command EPS_ListenToRadio (
            listen: U8 @< 1 to listen, 0 to stop listening
        )

        @ Command to tell eps to reset  
        async command EPS_TriggerReset

        @ Command to tell eps to adjust a parameter 
        async command EPS_AdjustParameter (
            parameter: U8 @< The parameter to adjust
            value: U8 @< The value to adjust the parameter to
        )

        # --- 
        # Events 
        # --- 

        @ Event for when a message is received from the EPS
        event EPS_UartMsgIn(
            msg: string size 40 @< The message received
                            ) \
            severity activity high \
            id 0 \
            format "Received msg {} from EPS"

        @ Event for when a message is sent to the EPS
        event EPS_UartMsgOut(
            msg: string size 40 @< The message sent
                            ) \
            severity activity high \
            id 1 \
            format "Sent msg {} to EPS"

        @ Event for when the EPS 
        event EPS_ListenToRadioEvent(
            listen: U8 @< 1 to listen, 0 to stop listening
                                    ) \
            severity activity high \
            id 2 \
            format "EPS is now listening to radio: {}"

        @ Event for when a bad packet is received from the EPS
        event EPS_BadPacket(
            msg: string size 40 @< The message received
                            ) \
            severity activity high \
            id 3 \
            format "Bad packet received from EPS: {}"

        @ event for when there is a bad checksum
        event EPS_BadChecksum(
            msg: string size 40 @< The message received
                            ) \
            severity activity high \
            id 4 \
            format "Bad checksum received from EPS: {}" 

        @ event for debugging ! // may remove at a future time 
        event EPS_DebugEvent(
            msg: string @< The message received
        ) \
        severity activity high \
        id 5 \
        format "debug: {}"


        # ---
        # Telemetry
        # ---

        @ Number of bytes sent on the UART
        telemetry EPS_UartRecvBytes: U32 \             # 4 bytes
        id 0

        @ Last received UART text message
        telemetry EPS_LastMsg: string size 32 \        # 32 bytes (tot: 36 bytes)
        id 1

        @ EPS Battery Voltage reading 
        telemetry EPS_VBattVoltage: F32 \              # 4 bytes (tot: 40 bytes)
        id 2

        @ EPS Battery Current reading
        telemetry EPS_VBattCurrent: F32 \              # 4 bytes (tot: 44 bytes)
        id 3

        @ EPS Battery Temperature reading
        telemetry EPS_CellVoltage: CELL_DATA \         # 16 bytes (tot: 60 bytes)
        id 4

        @ EPS Battery Current reading
        telemetry EPS_CellCurrent: CELL_DATA \         # 4 bytes (tot: 64 bytes)
        id 5    

        @ EPS Battery Temperature reading
        telemetry EPS_BattTemp: F32 \                  # 4 bytes  (tot: 68 bytes)
        id 6

        @ EPS Heater Status reading
        telemetry EPS_HeaterStatus: U8 \               # 1 byte (tot: 69 bytes)
        id 7

        @ EPS Solar Panel Voltage reading
        telemetry EPS_SolarPanelVoltage: PANEL_DATA \  # 16 bytes (tot: 85 bytes)
        id 8

        @ EPS Solar Panel Current reading
        telemetry EPS_SolarPanelCurrent: F32 \         # 4 bytes (tot: 89 bytes)
        id 9

        @ EPS Solar Panel Temperature reading
        telemetry EPS_SolarPanelTemp: F32 \            # 4 bytes (tot: 93 bytes)
        id 10

        @ EPS Voltage5V0 reading
        telemetry EPS_Voltage5v0: F32 \                # 4 bytes (tot: 97 bytes)
        id 11

        @ EPS Current5V0 reading
        telemetry EPS_Current5v0: F32 \                # 4 bytes (tot: 101 bytes)
        id 12

        @ EPS  voltage3v3 reading
        telemetry EPS_Voltage3v3: F32 \                # 4 bytes (tot: 105 bytes)
        id 13

        @ EPS  current3v3 reading
        telemetry EPS_Current3v3: F32 \                # 4 bytes (tot: 109 bytes)
        id 14

        @ EPS switch state reading
        telemetry EPS_SwitchState: U8 \                # 1 byte (tot: 110 bytes)
        id 15

    }

}