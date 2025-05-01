// ======================================================================
// \title  FileRecyclerTester.hpp
// \author g83r
// \brief  hpp file for FileRecycler component test harness implementation class
// ======================================================================

#ifndef FileRecyclerModule_FileRecyclerTester_HPP
#define FileRecyclerModule_FileRecyclerTester_HPP

#include "Components/FileRecycler/FileRecycler.hpp"
#include "Components/FileRecycler/FileRecyclerGTestBase.hpp"

namespace FileRecyclerModule {

class FileRecyclerTester : public FileRecyclerGTestBase {

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

  //! Construct object FileRecyclerTester
  FileRecyclerTester();

  //! Destroy object FileRecyclerTester
  ~FileRecyclerTester();

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
  FileRecycler component;
};

} // namespace FileRecyclerModule

#endif
