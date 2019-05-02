#include "TestingCore.hxx"


TEST(mustAllocateMemoryNotMoreThanOneTimePer_N_elements, mustAllocateMemoryNotMoreThanOneTimePer_N_elements){
    using namespace std;
    ASSERT_TRUE(mustAllocateMemoryNotMoreThanOneTimePer_N_elements<size_t,size_t,10,0>());
}
TEST(mustAllocateAndDeallocateMemorySameTimes, mustAllocateAndDeallocateMemorySameTimes){
    using namespace std;
    ASSERT_TRUE(mustAllocateAndDeallocateMemorySameTimes<size_t,size_t,10,0>());
}
TEST(memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly, memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly){
    using namespace std;
    ASSERT_TRUE(memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly<size_t,size_t,10,0,3>());
}
TEST(customSListContainerTest, customSListContainerTest){
    using namespace std;
    ASSERT_TRUE(customSListContainerTest<size_t,size_t,10,0>());
}
