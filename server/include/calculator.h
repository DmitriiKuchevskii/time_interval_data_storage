#pragma once
//
// Created by dmitrii on 1/31/20.
//

#ifndef TIME_INTERVAL_SUM_CALCULATOR_H
#define TIME_INTERVAL_SUM_CALCULATOR_H

#include <utility>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstring>

#include "timeprovider.h"

using namespace std::chrono;

template <typename DataType>
class ITimeIntervalSumCalculator
{
public:
    virtual ~ITimeIntervalSumCalculator() {}

    // Puts new element into time interval calculator
    virtual void put(const DataType& element) = 0;

    // Returns sum at the moment of the last insertion
    virtual DataType get() const = 0;
};
template <typename T>
using ITimeIntervalSumCalculatorPtr = std::shared_ptr<ITimeIntervalSumCalculator<T>>;

template <typename DataType>
class TimeIntervalSumCalculator : public ITimeIntervalSumCalculator<DataType>
{
public:
    explicit TimeIntervalSumCalculator(size_t intervalInMilliseconds, size_t maxUnusedSize = 1024 * 1024 * 5) :
        TimeIntervalSumCalculator(
            intervalInMilliseconds
          , maxUnusedSize
          , std::make_shared<StdTimeProvider>())
    {}

    TimeIntervalSumCalculator(size_t intervalInMilliseconds, size_t maxUnusedSize, ITimeProviderPtr timeProvider) :
          m_interval(intervalInMilliseconds)
        , m_maxUnusedSize(maxUnusedSize)
        , m_timeProvider(std::move(timeProvider))
    {}

    void put(const DataType& element) override
    {
        size_t timeNow = m_timeProvider->now();
        size_t boundTime = timeNow - m_interval;
        auto it = std::upper_bound(m_data.begin(), m_data.end(), boundTime,
                                   [](const auto& p1, const auto& e2) { return p1 < e2.first; });

        if (it == m_data.end())
        {
            m_curSubstruct = 0;
            m_data = { { timeNow, element } };
            return;
        }
        else if (it != m_data.begin())
        {
            m_curSubstruct = (it - 1)->second;
            if (std::distance(m_data.begin(), it) > m_maxUnusedSize)
            {
                m_data = { it, m_data.end() };
                for (auto& val : m_data)
                    val.second -= m_curSubstruct;
                m_curSubstruct = 0;
            }
        }

        m_data.emplace_back(timeNow, m_data.back().second + element);
    }

    [[nodiscard]] DataType get() const override
    {
        return m_data.back().second - m_curSubstruct;
    }

private:
    std::vector<std::pair<size_t, DataType>> m_data;
    const size_t m_interval;
    DataType m_curSubstruct =  0;
    const size_t m_maxUnusedSize;
    const ITimeProviderPtr m_timeProvider;
};


#endif //TIME_INTERVAL_SUM_CALCULATOR_H
