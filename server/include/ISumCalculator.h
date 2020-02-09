#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_ITIMEINTERVALSUMCALCULATOR_H
#define TIME_INTERVAL_SUM_SERVER_ITIMEINTERVALSUMCALCULATOR_H

template <typename DataType>
class ISumCalculator
{
public:
    virtual ~ISumCalculator() {}

    // Puts new element into time interval calculator
    virtual void put(const DataType& element) = 0;

    // Returns sum at the moment of the last insertion
    virtual DataType get() const = 0;
};
template <typename DataType>
using ITimeIntervalSumCalculatorPtr = std::shared_ptr<ISumCalculator<DataType>>;


#endif //TIME_INTERVAL_SUM_SERVER_ITIMEINTERVALSUMCALCULATOR_H
