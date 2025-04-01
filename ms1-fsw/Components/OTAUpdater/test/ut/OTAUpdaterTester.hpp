// ======================================================================
// \title  OTAUpdaterTester.hpp
// \author g83r
// \brief  hpp file for OTAUpdater component test harness implementation class
// ======================================================================

#ifndef UpdaterModule_OTAUpdaterTester_HPP
#define UpdaterModule_OTAUpdaterTester_HPP

#include "Components/OTAUpdater/OTAUpdater.hpp"
#include "Components/OTAUpdater/OTAUpdaterGTestBase.hpp"

namespace UpdaterModule {

class OTAUpdaterTester : public OTAUpdaterGTestBase {

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

  //! Construct object OTAUpdaterTester
  OTAUpdaterTester();

  //! Destroy object OTAUpdaterTester
  ~OTAUpdaterTester();

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
  OTAUpdater component;
};

} // namespace UpdaterModule

#endif
