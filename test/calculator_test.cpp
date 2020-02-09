//
// Created by dkuch on 07.02.2020.
//

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "calculator.h"

class TimeIntervalCalculatorTest : public testing::Test
{
private:
    struct TestTimeProvider : public ITimeProvider
    {
        explicit TestTimeProvider(size_t startTime)
            : m_testTimeNow(startTime)
        {}

        [[nodiscard]] size_t now() override
        {
            return m_testTimeNow;
        }

        void setTime(size_t millSeconds)
        {
            m_testTimeNow = millSeconds;
        }

    private:
        size_t m_testTimeNow;
    };

    std::shared_ptr<TestTimeProvider> m_timeProvider = std::make_shared<TestTimeProvider>(
        kCalculatorTimeIntervalInMilliseconds * 2
    );

protected:
    void waitFor(size_t millSeconds)
    {
        m_timeProvider->setTime(m_timeProvider->now() + millSeconds);
    }

    void waitForOneTimeInterval()
    {
        waitFor(kCalculatorTimeIntervalInMilliseconds);
    }

protected:
    static constexpr size_t kCalculatorTimeIntervalInMilliseconds = 60 * 1000;
    ITimeIntervalSumCalculatorPtr<int64_t> m_calculator = std::make_shared<TimeIntervalSumCalculator<int64_t>>(
        kCalculatorTimeIntervalInMilliseconds, 1024, m_timeProvider
    );
};


TEST_F(TimeIntervalCalculatorTest, SumDuringOneIntervalTest)
{
    for (size_t i = 0; i < 1000; ++i)
    {
        m_calculator->put(100);
    }
    ASSERT_EQ(m_calculator->get(), 100 * 1000);

    waitForOneTimeInterval();

    m_calculator->put(0);
    ASSERT_EQ(m_calculator->get(), 0);
}

TEST_F(TimeIntervalCalculatorTest, SumDuringInterceptTimeIntervalsTest)
{
    static_assert(kCalculatorTimeIntervalInMilliseconds % 3 == 0);

    m_calculator->put(100);
    waitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator->put(100);
    waitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator->put(100);
    ASSERT_EQ(m_calculator->get(), 300);
    waitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator->put(0);
    ASSERT_EQ(m_calculator->get(), 200);
    waitFor(2 * kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator->put(0);
    ASSERT_EQ(m_calculator->get(), 0);
}

TEST_F(TimeIntervalCalculatorTest, SumAfterLongTimeWithUniformInsertDistributionTest)
{
    for (size_t i = 0; i < 100000; ++i)
    {
        m_calculator->put(1);
        waitFor(1000);
    }
    ASSERT_EQ(m_calculator->get(), kCalculatorTimeIntervalInMilliseconds / 1000);
}

TEST_F(TimeIntervalCalculatorTest, SumDuringContiniusExpirityTest)
{
    for (size_t i = 0; i < kCalculatorTimeIntervalInMilliseconds; ++i)
    {
        m_calculator->put(1);
        waitFor(1);
    }

    for (size_t i = 1 ; i < kCalculatorTimeIntervalInMilliseconds; ++i)
    {
        m_calculator->put(0);
        ASSERT_EQ(m_calculator->get(), kCalculatorTimeIntervalInMilliseconds - i);
        waitFor(1);
    }
}

TEST_F(TimeIntervalCalculatorTest, SumWithNegativeNumbersTest)
{
    for (size_t i = 0; i < 100; ++i)
    {
        m_calculator->put(1);
    }
    ASSERT_EQ(m_calculator->get(), 100);

    for (size_t i = 0; i < 100; ++i)
    {
        m_calculator->put(-1);
    }
    ASSERT_EQ(m_calculator->get(), 0);
}

TEST_F(TimeIntervalCalculatorTest, SumWithRealloctionTest)
{
    for (size_t i = 0; i < 10000; ++i)
    {
        m_calculator->put(1);
    }
    waitFor(2 * kCalculatorTimeIntervalInMilliseconds / 3);
    for (size_t i = 0; i < 1000; ++i)
    {
        m_calculator->put(1);
    }
    waitFor(kCalculatorTimeIntervalInMilliseconds / 3);
    m_calculator->put(0);
    ASSERT_EQ(m_calculator->get(), 1000);

    for (size_t i = 0; i < 1000; ++i)
    {
        m_calculator->put(-1);
    }
    ASSERT_EQ(m_calculator->get(), 0);
}




