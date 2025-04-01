# ms1/Types/EpsUartTypes.fpp

# Module 
module EpsModule { 

    @ EpsUart Message Types 
    enum MessageType { 
        NOP = 0x00,
        ACK = 0x01,
        ERROR = 0x07,
        RESET = 0x80,

        GET_DATA = 0x02,
        RETURN_DATA = 0x03,
        SET_VALUE = 0x04,
        RESEND = 0x05
    }

    @ EpsUart Sensor Data 
    enum SensorData {
        VBATT_VOLTAGE = 0x00,
        VBATT_CURRENT = 0x01,
        CELL_VOLTAGE = 0x02,
        CELL_CURRENT = 0x03,
        BATTERY_TEMP = 0x04,
        HEATER_STATUS = 0x05,
        SOLAR_PANEL_VOLTAGE = 0x06,
        SOLAR_PANEL_CURRENT = 0x07,
        VOLTAGE_5V0 = 0x08,
        CURRENT_5V0 = 0x09,
        VOLTAGE_3V3 = 0x0A,
        CURRENT_3V3 = 0x0B,
        SWITCH_STATE = 0x0C,
        SENSOR_DATA = 0x0D,
        FSM_STATE = 0x0E
    }

    @ EpsUart State 
    enum EPS_State {
        STATE_INIT = 0x00,
        STATE_IDLE = 0x01,
        STATE_CRUISE = 0x02,
        STATE_ERROR = 0x03,
        STATE_LOW_PWR = 0x04,
        STATE_STM_CTL = 0x05, 
        SAFE_ANOM = 0x06,
        RESTART = 0x07
    }

    @ EpsUart Command 
    #! Note
    #! The command values are arbitrary and are not defined in the EPS documentation.
    #! The port definitions will also need to be taken into consideration
    enum EPS_CommandRecv {
        HEARTBEAT_PKT, 
        HEARTBEAT_ACK, 
        CRUISE_PKT, 
        IDLE_PKT, 
        RESET_PKT
    }

    enum EPS_CommandSend {
        HEARTBEAT_ACK, 
        STATE_INIT, 
        STATE_IDLE, 
        STATE_CRUISE, 
        STATE_ERROR, 
        STATE_LO_PWR, 
        STATE_STM_CTL,  
        SAFE_ANOM, 
        RESTART
    }

    array CELL_DATA = [2] I64;
    array PANEL_DATA = [2] I64;

}
