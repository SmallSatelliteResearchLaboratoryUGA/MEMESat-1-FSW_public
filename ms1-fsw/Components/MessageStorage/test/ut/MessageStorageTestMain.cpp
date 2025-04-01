// ======================================================================
// \title  MessageStorageTestMain.cpp
// \author g83r
// \brief  cpp file for MessageStorage component test main function
// ======================================================================

#include "MessageStorageTester.hpp"

TEST(Nominal, toDo) {
  MessageModule::MessageStorageTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
