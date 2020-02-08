//
// Created by dkuch on 07.02.2020.
//

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "calculator.h"

class TimeIntervalCalculatorTest : public testing::Test
{
public:
    void SetTime(size_t millSeconds)
    {
        m_testTimeNow = millSeconds;
    }

    [[nodiscard]] size_t Now() const
    {
        return m_testTimeNow;
    }

    void WaitFor(size_t millSeconds)
    {
        m_testTimeNow += millSeconds;
    }

    void WaitForOneTimeInterval()
    {
        m_testTimeNow += kCalculatorTimeIntervalInMilliseconds;
    }

protected:
    static constexpr size_t kCalculatorTimeIntervalInMilliseconds = 60 * 1000;
    TimeIntervalSumCalculator<int64_t, TimeIntervalCalculatorTest> m_calculator{
            kCalculatorTimeIntervalInMilliseconds, 1024 * 1024 * 5, *this
    };

private:
    size_t m_testTimeNow = kCalculatorTimeIntervalInMilliseconds * 2;
};


TEST_F(TimeIntervalCalculatorTest, SumDuringOneIntervalTest)
{
    for (size_t i = 0; i < 1000; ++i)
    {
        m_calculator.put(100);
    }
    ASSERT_EQ(m_calculator.get(), 100 * 1000);

    WaitForOneTimeInterval();

    m_calculator.put(0);
    ASSERT_EQ(m_calculator.get(), 0);
}

TEST_F(TimeIntervalCalculatorTest, SumDuringInterceptTimeIntervalsTest)
{
    static_assert(kCalculatorTimeIntervalInMilliseconds % 3 == 0);

    m_calculator.put(100);
    WaitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator.put(100);
    WaitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator.put(100);
    ASSERT_EQ(m_calculator.get(), 300);
    WaitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator.put(0);
    ASSERT_EQ(m_calculator.get(), 200);
    WaitFor(2 * kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator.put(0);
    ASSERT_EQ(m_calculator.get(), 0);
}

TEST_F(TimeIntervalCalculatorTest, SumAfterLongTimeWithUniformInsertDistributionTest)
{
    for (size_t i = 0; i < 100000; ++i)
    {
        m_calculator.put(1);
        WaitFor(1000);
    }
    ASSERT_EQ(m_calculator.get(), kCalculatorTimeIntervalInMilliseconds / 1000);
}

TEST_F(TimeIntervalCalculatorTest, SumDuringContiniusExpirityTest)
{
    for (size_t i = 0; i < kCalculatorTimeIntervalInMilliseconds; ++i)
    {
        m_calculator.put(1);
        WaitFor(1);
    }

    for (size_t i = 1 ; i < kCalculatorTimeIntervalInMilliseconds; ++i)
    {
        m_calculator.put(0);
        ASSERT_EQ(m_calculator.get(), kCalculatorTimeIntervalInMilliseconds - i);
        WaitFor(1);
    }
}

TEST_F(TimeIntervalCalculatorTest, SumWithNegativeNumbersTest)
{
    for (size_t i = 0; i < 100; ++i)
    {
        m_calculator.put(1);
    }
    ASSERT_EQ(m_calculator.get(), 100);

    for (size_t i = 0; i < 100; ++i)
    {
        m_calculator.put(-1);
    }
    ASSERT_EQ(m_calculator.get(), 0);
}




