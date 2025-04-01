// ======================================================================
// \title  FileRecyclerTester.cpp
// \author g83r
// \brief  cpp file for FileRecycler component test harness implementation class
// ======================================================================

#include "FileRecyclerTester.hpp"

namespace FileRecyclerModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FileRecyclerTester ::FileRecyclerTester()
    : FileRecyclerGTestBase("FileRecyclerTester",
                            FileRecyclerTester::MAX_HISTORY_SIZE),
      component("FileRecycler") {
  this->initComponents();
  this->connectPorts();
}

FileRecyclerTester ::~FileRecyclerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FileRecyclerTester ::toDo() {
  // TODO
}

} // namespace FileRecyclerModule
