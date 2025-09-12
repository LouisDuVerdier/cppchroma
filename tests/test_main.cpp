#include <gtest/gtest.h>
#include "../src/cppchroma.cpp"

TEST(MainTest, ExternalProgramCall)
{
    char* argv[] = { (char *)"ls" };
    int argc = sizeof(argv) / sizeof(argv[0]);

    int result = run(argc, argv);
    EXPECT_EQ(result, 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
