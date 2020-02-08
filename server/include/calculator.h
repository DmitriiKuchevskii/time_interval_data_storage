#pragma once
//
// Created by dmitrii on 1/31/20.
//

#ifndef TIME_INTERVAL_SUM_CALCULATOR_H
#define TIME_INTERVAL_SUM_CALCULATOR_H

#include <vector>
#include <chrono>
#include <algorithm>
#include <cstring>

using namespace std::chrono;

class StdTimeProvider
{
public:
    static size_t Now()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }
};


template <typename DataType, typename TimeProvider = StdTimeProvider>
class TimeIntervalSumCalculator
{
public:
    explicit TimeIntervalSumCalculator(
          size_t intervalInMilliseconds
        , size_t maxUnusedSize = 1024 * 1024 * 5
        , const TimeProvider& timeProvider = TimeProvider{}) :
            kDataTimeIntervalInMilliseconds(intervalInMilliseconds)
          , kMaxUnusedSize(maxUnusedSize)
          , m_timeProvider(timeProvider)
    {}

    void put(DataType number)
    {
        size_t timeNow = m_timeProvider.Now();
        size_t boundTime = timeNow - kDataTimeIntervalInMilliseconds;
        auto it = std::upper_bound(m_data.begin(), m_data.end(), boundTime,
                                   [](const auto& p1, const auto& e2) { return p1 < e2.first; });

        if (it == m_data.end())
        {
            m_curSubstruct = 0;
            m_data = { { timeNow, number } };
            return;
        }
        else if (it != m_data.begin())
        {
            m_curSubstruct = (it - 1)->second;
            if (std::distance(m_data.begin(), it) > kMaxUnusedSize)
            {
                m_data = { it, m_data.end() };
                for (auto& val : m_data)
                    val.second -= m_curSubstruct;
                m_curSubstruct = 0;
            }
        }

        m_data.emplace_back(timeNow, m_data.back().second + number);
    }

    [[nodiscard]] DataType get() const
    {
        return m_data.back().second - m_curSubstruct;
    }

private:
    std::vector<std::pair<size_t, DataType>> m_data;
    const size_t kDataTimeIntervalInMilliseconds;
    DataType m_curSubstruct =  0;
    const size_t kMaxUnusedSize;
    const TimeProvider& m_timeProvider;
};


#endif //TIME_INTERVAL_SUM_CALCULATOR_H
