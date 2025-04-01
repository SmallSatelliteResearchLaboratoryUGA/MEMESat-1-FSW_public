// ======================================================================
// \title  FileRecyclerTestMain.cpp
// \author g83r
// \brief  cpp file for FileRecycler component test main function
// ======================================================================

#include "FileRecyclerTester.hpp"

TEST(Nominal, toDo) {
  FileRecyclerModule::FileRecyclerTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
