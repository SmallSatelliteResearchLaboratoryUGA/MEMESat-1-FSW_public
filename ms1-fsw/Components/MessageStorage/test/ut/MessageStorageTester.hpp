// ======================================================================
// \title  MessageStorageTester.hpp
// \author g83r
// \brief  hpp file for MessageStorage component test harness implementation
// class
// ======================================================================

#ifndef MessageModule_MessageStorageTester_HPP
#define MessageModule_MessageStorageTester_HPP

#include "Components/MessageStorage/MessageStorage.hpp"
#include "Components/MessageStorage/MessageStorageGTestBase.hpp"

namespace MessageModule {

class MessageStorageTester : public MessageStorageGTestBase {

public:
  // ----------------------------------------------------------------------
  // Constants
  // ----------------------------------------------------------------------

  // Maximum size of histories storing events, telemetry, and port outputs
  static const FwSizeType MAX_HISTORY_SIZE = 10;

  // Instance ID supplied to the component instance under test
  static const FwEnumStoreType TEST_INSTANCE_ID = 0;

public:
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  //! Construct object MessageStorageTester
  MessageStorageTester();

  //! Destroy object MessageStorageTester
  ~MessageStorageTester();

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
  MessageStorage component;
};

} // namespace MessageModule

#endif
