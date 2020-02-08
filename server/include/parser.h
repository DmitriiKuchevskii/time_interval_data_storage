//
// Created by dkuch on 07.02.2020.
//

#ifndef TIME_INTERVAL_SUM_SERVER_NUMBERSSTREAMPARSER_H
#define TIME_INTERVAL_SUM_SERVER_NUMBERSSTREAMPARSER_H

#include <charconv>

class ParserException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

template <typename InputNumberType>
class NumbersStreamParser
{
public:
    NumbersStreamParser(char msgDelimiter, size_t backlogSize) :
          kDelimiter(msgDelimiter)
        , m_backlogData(backlogSize)
    {}

public:
    std::optional<InputNumberType> Parse(const void* data, size_t size)
    {
        m_parseBuffer = { (const char*)data, size };
        m_parseBuffPos = 0;

        auto delimiterPos = m_parseBuffer.find(kDelimiter);
        if (delimiterPos == std::string::npos)
        {
            AddToBacklog(m_parseBuffer.data(), m_parseBuffer.size());
            return {};
        }

        return m_backlogPos ? GetBacklogValue(delimiterPos) : GetNextValue(delimiterPos);
    }

    std::optional<InputNumberType> Next()
    {
        auto endPos = m_parseBuffer.find(kDelimiter, m_parseBuffPos);
        if (endPos == std::string::npos)
        {
            AddToBacklog(m_parseBuffer.data() + m_parseBuffPos, m_parseBuffer.size() - m_parseBuffPos);
            return {};
        }

        return GetNextValue(endPos - m_parseBuffPos);
    }

private:
    void AddToBacklog(const char* data, size_t size)
    {
        if (m_backlogPos + size >= m_backlogData.size())
        {
            throw ParserException("Backlog overflow. Too big number or incorrect format.");
        }
        memcpy(&m_backlogData[m_backlogPos], data, size);
        m_backlogPos += size;
    }

    InputNumberType GetBacklogValue(size_t size)
    {
        AddToBacklog(m_parseBuffer.data(), size);
        auto backlogValue = ParseNumber(m_backlogData.data(), m_backlogPos);
        m_parseBuffPos = size + 1;
        m_backlogPos = 0;
        return backlogValue;
    }

    InputNumberType ParseNumber(const char* value, size_t size) const
    {
        while(std::isspace(static_cast<unsigned char>(*value)) && size) { value++; size--; };

        InputNumberType result;
        if(auto [next, error] = std::from_chars(value, value + size, result); error == std::errc())
        {
            while(next != value + size)
            {
                if (!std::isspace(static_cast<unsigned char>(*next++)))
                    throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
            }
            return result;
        }

        throw ParserException("Invalid message format: '" + std::string(value, size) + "'");
    }

    InputNumberType GetNextValue(size_t size)
    {
        auto parsedValue = ParseNumber(m_parseBuffer.data() + m_parseBuffPos, size);
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
