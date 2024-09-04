#pragma once

#include <gtest/gtest.h>
#include "fuzzing.hpp"
#include <tgbm/tools/formatters.hpp>

#ifndef TGBM_BEGIN_CHAOTIC
#define TGBM_BEGIN_CHAOTIC std::size_t(0)
#endif

#ifndef TGBM_END_CHAOTIC
#define TGBM_END_CHAOTIC std::size_t(100)
#endif

inline thread_local std::mt19937_64 chaotic_generator(0);

struct GeneratorTest : testing::Test{
    void SetUp() override{
        chaotic_generator.seed(0);
    }
    void TearDown() override{
        chaotic_generator.seed(0);
    }

    template <typename T>
    auto range(){
        return fuzzing::range<T>(TGBM_BEGIN_CHAOTIC, TGBM_END_CHAOTIC, chaotic_generator);
    }
};

#define FUZZING_TEST(TestName) TEST_F(GeneratorTest, TestName)
