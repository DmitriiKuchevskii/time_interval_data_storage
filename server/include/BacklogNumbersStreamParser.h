//
// Created by dkuch on 07.02.2020.
//

#ifndef TIME_INTERVAL_SUM_SERVER_NUMBERSSTREAMPARSER_H
#define TIME_INTERVAL_SUM_SERVER_NUMBERSSTREAMPARSER_H

#include <charconv>

#include "INumbersStreamParser.h"

template <typename InputNumberType>
class BacklogNumbersStreamParser : public INumbersStreamParser<InputNumberType>
{
public:
    BacklogNumbersStreamParser(char msgDelimiter, size_t backlogSize) :
          kDelimiter(msgDelimiter)
        , m_backlogData(backlogSize)
    {}

public:
    std::optional<InputNumberType> parse(const void* data, size_t size) override
    {
        m_parseBuffer = { (const char*)data, size };
        m_parseBuffPos = 0;

        auto delimiterPos = m_parseBuffer.find(kDelimiter);
        if (delimiterPos == std::string::npos)
        {
            addToBacklog(m_parseBuffer.data(), m_parseBuffer.size());
            return {};
        }

        return m_backlogPos ? getBacklogValue(delimiterPos) : getNextValue(delimiterPos);
    }

    std::optional<InputNumberType> next() override
    {
        auto endPos = m_parseBuffer.find(kDelimiter, m_parseBuffPos);
        if (endPos == std::string::npos)
        {
            addToBacklog(m_parseBuffer.data() + m_parseBuffPos, m_parseBuffer.size() - m_parseBuffPos);
            return {};
        }

        return getNextValue(endPos - m_parseBuffPos);
    }

private:
    void addToBacklog(const char* data, size_t size)
    {
        if (m_backlogPos + size >= m_backlogData.size())
        {
            throw ParserException("Backlog overflow. Too big number or incorrect format.");
        }
        memcpy(&m_backlogData[m_backlogPos], data, size);
        m_backlogPos += size;
    }

    InputNumberType getBacklogValue(size_t size)
    {
        addToBacklog(m_parseBuffer.data(), size);
        auto backlogValue = parseNumber(m_backlogData.data(), m_backlogPos);
        m_parseBuffPos = size + 1;
        m_backlogPos = 0;
        return backlogValue;
    }

    InputNumberType parseNumber(const char* value, size_t size) const
    {
        InputNumberType result;

        if constexpr (std::numeric_limits<InputNumberType>::is_integer)
        {
            while (std::isspace(static_cast<unsigned char>(*value)) && size)
            {
                value++;
                size--;
            };

            if (auto[next, error] = std::from_chars(value, value + size, result); error == std::errc())
            {
                while (next != value + size)
                {
                    if (!std::isspace(static_cast<unsigned char>(*next++)))
                        throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
                }
                return result;
            }
            throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
        }
        else // GCC doe not implement from_chars for not-integer types yet
        {
            char* end;
            result = std::strtod(value, &end);

            if (errno == ERANGE)
            {
                errno = 0;
                throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
            }

            if (value == end && result == 0.0)
            {
                throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
            }

            while (end != value + size)
            {
                if (!std::isspace(static_cast<unsigned char>(*end++)))
                    throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
            }

            return result;
        }
    }

    InputNumberType getNextValue(size_t size)
    {
        auto parsedValue = parseNumber(m_parseBuffer.data() + m_parseBuffPos, size);
        m_parseBuffPos += size + 1;
        return parsedValue;
    }

protected:
    std::string_view m_parseBuffer;
    const char kDelimiter;
    std::vector<char> m_backlogData;
    size_t m_backlogPos = 0;
    size_t m_parseBuffPos = 0;
};

#endif //TIME_INTERVAL_SUM_SERVER_NUMBERSSTREAMPARSER_H
