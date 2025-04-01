// ======================================================================
// \title  OTAUpdaterTestMain.cpp
// \author g83r
// \brief  cpp file for OTAUpdater component test main function
// ======================================================================

#include "OTAUpdaterTester.hpp"

TEST(Nominal, toDo) {
  UpdaterModule::OTAUpdaterTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
