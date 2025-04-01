// ======================================================================
// \title  SchedulerTester.hpp
// \author g83r
// \brief  hpp file for Scheduler component test harness implementation class
// ======================================================================

#ifndef SchedulerModule_SchedulerTester_HPP
#define SchedulerModule_SchedulerTester_HPP

#include "Components/Scheduler/Scheduler.hpp"
#include "Components/Scheduler/SchedulerGTestBase.hpp"

namespace SchedulerModule {

class SchedulerTester : public SchedulerGTestBase {

public:
  // ----------------------------------------------------------------------
  // Constants
  // ----------------------------------------------------------------------

  // Maximum size of histories storing events, telemetry, and port outputs
  static const FwSizeType MAX_HISTORY_SIZE = 10;

  // Instance ID supplied to the component instance under test
  static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  // Queue depth supplied to the component instance under test
  static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

public:
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  //! Construct object SchedulerTester
  SchedulerTester();

  //! Destroy object SchedulerTester
  ~SchedulerTester();

public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! To do
  void toDo();

private:
  // ----------------------------------------------------------------------
  // Helper functions
  // ----------------------------------------------------------------------

  //! Connect ports
  void connectPorts();

  //! Initialize components
  void initComponents();

private:
  // ----------------------------------------------------------------------
  // Member variables
  // ----------------------------------------------------------------------

  //! The component under test
  Scheduler component;
};

} // namespace SchedulerModule

#endif
