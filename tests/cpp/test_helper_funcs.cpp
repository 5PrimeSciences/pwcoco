#include "helper_funcs.h"

#include <gtest/gtest.h>

#include <cmath>
#include <vector>

TEST(PwCoCoHelperFuncs, String2Upper) {
    EXPECT_EQ(string2upper("rs12345"), "RS12345");
    EXPECT_EQ(string2upper("AaBb"), "AABB");
}

TEST(PwCoCoHelperFuncs, IsFloatEqual) {
    EXPECT_TRUE(isFloatEqual(1.0, 1.0));
    EXPECT_FALSE(isFloatEqual(1.0, 1.1));
}

TEST(PwCoCoHelperFuncs, Logsum) {
    const std::vector<double> values{0.0, log(2.0)};
    EXPECT_NEAR(logsum(values), log(3.0), 1e-9);
}
