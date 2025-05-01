// ======================================================================
// \title  StateMachineTester.hpp
// \author g83r
// \brief  hpp file for StateMachine component test harness implementation class
// ======================================================================

#ifndef StateMachineModule_StateMachineTester_HPP
#define StateMachineModule_StateMachineTester_HPP

#include "Components/StateMachine/StateMachine.hpp"
#include "Components/StateMachine/StateMachineGTestBase.hpp"

namespace StateMachineModule {

class StateMachineTester : public StateMachineGTestBase {

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

  //! Construct object StateMachineTester
  StateMachineTester();

  //! Destroy object StateMachineTester
  ~StateMachineTester();

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
  StateMachine component;
};

} // namespace StateMachineModule

#endif
