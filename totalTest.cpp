#include "TestingCore.hxx"

TEST(mustAllocateMemoryNotMoreThanOneTimePer_N_elements, mustAllocateMemoryNotMoreThanOneTimePer_N_elements){
    using namespace std;
    bool mustBeTrue = mustAllocateMemoryNotMoreThanOneTimePer_N_elements<size_t,size_t,10,0>();
    ASSERT_TRUE(mustBeTrue);
}
TEST(mustAllocateAndDeallocateMemorySameTimes, mustAllocateAndDeallocateMemorySameTimes){
    using namespace std;
    bool mustBeTrue = mustAllocateAndDeallocateMemorySameTimes<size_t,size_t,10,0>();
    ASSERT_TRUE(mustBeTrue);
}
TEST(memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly, memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly){
    using namespace std;
    bool mustBeTrue = memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly<size_t,size_t,10,0,3>();
    ASSERT_TRUE(mustBeTrue);
}
TEST(customSListContainerTest, customSListContainerTest){
    using namespace std;
    bool mustBeTrue = customSListContainerTest<size_t,size_t,10,0>();
    ASSERT_TRUE(mustBeTrue);
}
