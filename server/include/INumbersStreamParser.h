#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_INUMBERSSTREAMPARSER_H
#define TIME_INTERVAL_SUM_SERVER_INUMBERSSTREAMPARSER_H

#include <optional>
#include <memory>

class ParserException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

template <typename InputNumberType>
class INumbersStreamParser
{
public:
    virtual ~INumbersStreamParser() {}

    // Parses input data stream. If any data exists returns the first element, otherwise std::nullopt returns.
    //
    // ParserException is thrown if data format is incorrect.
    virtual std::optional<InputNumberType> parse(const void* dataStream, size_t size) = 0;

    // Returns next element in the stream. If no elements available std::nullopt returns.
    //
    // NOTE: 1. Once this function returned std::nullopt you can only call it again after parse method called,
    //          otherwise it will lead to undefined behavior.
    //       2. If parse method returned std::noopt calling this function causes undefined behavior.
    //
    // ParserException is thrown if data format is incorrect
    virtual std::optional<InputNumberType> next() = 0;
};
template <typename DataType>
using INumbersStreamParserPtr = std::shared_ptr<INumbersStreamParser<DataType>>;

#endif //TIME_INTERVAL_SUM_SERVER_INUMBERSSTREAMPARSER_H
