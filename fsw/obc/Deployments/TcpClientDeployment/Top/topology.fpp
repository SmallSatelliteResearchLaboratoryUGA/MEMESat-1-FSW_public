module TcpClientDeployment {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
    rateGroup2
    rateGroup3
  }

  topology TcpClientDeployment {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance blockDrv
    instance tlmSend
    instance cmdDisp
    instance cmdSeq
    instance comDriver
    instance comQueue
    instance comStub
    instance deframer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileManager
    instance fileUplink
    instance bufferManager
    instance framer
    instance posixTime
    instance prmDb
    instance rateGroup1
    instance rateGroup2
    instance rateGroup3
    instance rateGroupDriver
    instance textLogger
    instance systemResources
    instance uartDriver       # uartDriver
    instance epsUart          # epsUart
    instance stateMachine     # stateMachine
    instance scheduler        # scheduler
    instance fileRecycler     # fileRecycler
    instance tlmChanWrapper   # tlmChanWrapper
    instance messageStorage   # messageStorage
    instance otaUpdater       # otaUpdater
    # instance gpio20Drv
    # instance gpio21Drv

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    param connections instance prmDb

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance posixTime

    health connections instance $health

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {

      eventLogger.PktSend -> comQueue.comQueueIn[0]
      tlmSend.PktSend -> comQueue.comQueueIn[1]
      fileDownlink.bufferSendOut -> comQueue.buffQueueIn[0]

      comQueue.comQueueSend -> framer.comIn
      comQueue.buffQueueSend -> framer.bufferIn

      framer.framedAllocate -> bufferManager.bufferGetCallee
      framer.framedOut -> comStub.comDataIn
      framer.bufferDeallocate -> fileDownlink.bufferReturn

      comDriver.deallocate -> bufferManager.bufferSendIn
      comDriver.ready -> comStub.drvConnected

      comStub.comStatus -> framer.comStatusIn
      framer.comStatusOut -> comQueue.comStatusIn
      comStub.drvDataOut -> comDriver.$send

    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections RateGroups {
      # Block driver
      blockDrv.CycleOut -> rateGroupDriver.CycleIn

      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> tlmSend.Run
      rateGroup1.RateGroupMemberOut[1] -> fileDownlink.Run
      rateGroup1.RateGroupMemberOut[2] -> systemResources.run
      rateGroup1.RateGroupMemberOut[3] -> scheduler.tick
      rateGroup1.RateGroupMemberOut[4] -> stateMachine.Run

      # Rate group 2
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2.CycleIn
      rateGroup2.RateGroupMemberOut[0] -> cmdSeq.schedIn

      # Rate group 3
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3.CycleIn
      rateGroup3.RateGroupMemberOut[0] -> $health.Run
      rateGroup3.RateGroupMemberOut[1] -> blockDrv.Sched
      rateGroup3.RateGroupMemberOut[2] -> bufferManager.schedIn
    }

    connections Sequencer {
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
    }

    connections Uplink {

      comDriver.allocate -> bufferManager.bufferGetCallee
      comDriver.$recv -> comStub.drvDataIn
      comStub.comDataOut -> deframer.framedIn

      deframer.framedDeallocate -> bufferManager.bufferSendIn
      deframer.comOut -> cmdDisp.seqCmdBuff

      cmdDisp.seqCmdStatus -> deframer.cmdResponseIn

      deframer.bufferAllocate -> bufferManager.bufferGetCallee
      deframer.bufferOut -> fileUplink.bufferSendIn
      deframer.bufferDeallocate -> bufferManager.bufferSendIn
      fileUplink.bufferSendOut -> bufferManager.bufferSendIn
    }

    connections TcpClientDeployment {

      #! (start) EPS Connections 
      epsUart.UartBuffers -> bufferManager.bufferSendIn
      epsUart.UartWrite -> uartDriver.$send
      uartDriver.$recv -> epsUart.UartRead
      uartDriver.allocate -> bufferManager.bufferGetCallee
      epsUart.SM_CommandBuffer -> stateMachine.SM_EpsCommandIn
      #! (end) EPS Connections

      #! (start) Scheduler Connections 
      scheduler.downlinkCurrentSchedules -> fileDownlink.SendFile 
      # Running Schedule 
      scheduler.runSchedule[0] -> fileRecycler.scheduledHandler 
      scheduler.runSchedule[1] -> tlmChanWrapper.scheduledHandler 
      scheduler.runSchedule[2] -> otaUpdater.scheduledHandler
      # Getting Schedule 
      fileRecycler.sendSchedule   -> scheduler.getSchedule[0]
      tlmChanWrapper.sendSchedule -> scheduler.getSchedule[1]
      otaUpdater.sendSchedule     -> scheduler.getSchedule[2]
      #! (end) Scheduler Connections

      #! (start) FileRecycler Connections
      fileRecycler.toFileRecyclerHandler -> fileRecycler.fileRecyclerOpIn 
      #! (end) FileRecycler Connections

      #! (start) TlmChanWrapper Connections
      tlmChanWrapper.tlmChanOut -> tlmSend.Run 
      #! (end) TlmChanWrapper Connections

      #! (start) StateMachine Connections
      stateMachine.SM_ReadyForPowerOff[0] -> fileRecycler.readyForPowerOff
      stateMachine.SM_ReadyForPowerOff[1] -> otaUpdater.readyForPowerOff  
      stateMachine.SM_EpsCommand          -> epsUart.ExternCommandBuffer
      #! (end) StateMachine Connections

      #! (start) OTAUpdater Component Connections 
      otaUpdater.cleanupDataPath -> fileRecycler.cleanupDataPathRecv
      #! (end) OTAUpdater Component Connections

      #! (start) MessageStorage Component Connections
      # {} -> messageStorage.storeMessage 
      # {} -> messageStorage.loadMessageFromIndex
      # {} -> messageStorage.loadMessageLastN
      #! (end) MessageStorage Component Connections

      #! (start) model gpio connections 
      # otaUpdater.gpioRead     -> gpio20Drv.gpioRead
      # otaUpdater.gpioWrite[0] -> gpio21Drv.gpioWrite
      # otaUpdater.gpioWrite[1] -> gpio22Drv.gpioWrite
      #! (end) model gpio connections



    }

  }

}