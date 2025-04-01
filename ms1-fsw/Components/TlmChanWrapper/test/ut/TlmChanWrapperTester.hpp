// ======================================================================
// \title  TlmChanWrapperTester.hpp
// \author g83r
// \brief  hpp file for TlmChanWrapper component test harness implementation
// class
// ======================================================================

#ifndef TlmChanModule_TlmChanWrapperTester_HPP
#define TlmChanModule_TlmChanWrapperTester_HPP

#include "Components/TlmChanWrapper/TlmChanWrapper.hpp"
#include "Components/TlmChanWrapper/TlmChanWrapperGTestBase.hpp"

namespace TlmChanModule {

class TlmChanWrapperTester : public TlmChanWrapperGTestBase {

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

  //! Construct object TlmChanWrapperTester
  TlmChanWrapperTester();

  //! Destroy object TlmChanWrapperTester
  ~TlmChanWrapperTester();

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
  TlmChanWrapper component;
};

} // namespace TlmChanModule

#endif
