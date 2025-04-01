// ======================================================================
// \title  EpsUartTester.hpp
// \author g83r
// \brief  hpp file for EpsUart component test harness implementation class
// ======================================================================

#ifndef EpsModule_EpsUartTester_HPP
#define EpsModule_EpsUartTester_HPP

#include "Components/EpsUart/EpsUart.hpp"
#include "Components/EpsUart/EpsUartGTestBase.hpp"

namespace EpsModule {

class EpsUartTester : public EpsUartGTestBase {

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

  //! Construct object EpsUartTester
  EpsUartTester();

  //! Destroy object EpsUartTester
  ~EpsUartTester();

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
  EpsUart component;
};

} // namespace EpsModule

#endif
