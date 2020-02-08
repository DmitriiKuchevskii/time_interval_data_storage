//
// Created by dkuch on 07.02.2020.
//

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "parser.h"

class IntegerParserTest : public testing::Test
{
protected:
    std::vector<int64_t> ParseMsg(const std::string& msg)
    {
        std::vector<int64_t> parsedValues;

        auto anyData = m_parser->Parse(msg.data(), msg.size());
        if (anyData)
        {
            parsedValues.push_back(*anyData);
            while(auto val = m_parser->Next())
            {
                parsedValues.push_back(*val);
            }
        }

        return parsedValues;
    }

    void Reset()
    {
        m_parser = std::make_unique<NumbersStreamParser<int64_t>>(kMsgDelimiter, kBacklogSize);
    }

protected:
    static constexpr char kMsgDelimiter = '\r';
    static constexpr size_t kBacklogSize = 32;
    std::unique_ptr<NumbersStreamParser<int64_t>> m_parser =
            std::make_unique<NumbersStreamParser<int64_t>>(kMsgDelimiter, kBacklogSize);
};

TEST_F(IntegerParserTest, EmptyMessageTest)
{
    ASSERT_THAT(ParseMsg(""), testing::ElementsAre());
}

TEST_F(IntegerParserTest, MessageWithSpacesOnlyTest)
{
    ASSERT_THAT(ParseMsg("   \t\t    \t   \t\t"), testing::ElementsAre());
}

TEST_F(IntegerParserTest, FullMesageWithOneValueWithoutSpacesTest)
{
    ASSERT_THAT(ParseMsg("0\n\r"), testing::ElementsAre(0));
    ASSERT_THAT(ParseMsg("1\n\r"), testing::ElementsAre(1));
    ASSERT_THAT(ParseMsg("100\n\r"), testing::ElementsAre(100));
    ASSERT_THAT(ParseMsg("9999\n\r"), testing::ElementsAre(9999));
}

TEST_F(IntegerParserTest, FullMesageWithOneValueWithSpacesTest)
{
    ASSERT_THAT(ParseMsg("\t\t\t   0\t\t\n\r    "), testing::ElementsAre(0));
    ASSERT_THAT(ParseMsg("\t\t\t   1\t\t\n\r    "), testing::ElementsAre(1));
    ASSERT_THAT(ParseMsg("\t\t\t   100\t\t\n\r    "), testing::ElementsAre(100));
    ASSERT_THAT(ParseMsg("\t\t\t   9999\t\t\n\r    "), testing::ElementsAre(9999));
}

TEST_F(IntegerParserTest, FullMesageWithOneNegativeValueWithoutSpacesTest)
{
    ASSERT_THAT(ParseMsg("-1\n\r"), testing::ElementsAre(-1));
    ASSERT_THAT(ParseMsg("-100\n\r"), testing::ElementsAre(-100));
    ASSERT_THAT(ParseMsg("-9999\n\r"), testing::ElementsAre(-9999));
}

TEST_F(IntegerParserTest, FullMesageWithOneNegativeValueWithSpacesTest)
{
    ASSERT_THAT(ParseMsg("\t\t\t   -1\t\t\n\r    "), testing::ElementsAre(-1));
    ASSERT_THAT(ParseMsg("\t\t\t   -100\t\t\n\r    "), testing::ElementsAre(-100));
    ASSERT_THAT(ParseMsg("\t\t\t   -9999\t\t\n\r    "), testing::ElementsAre(-9999));
}

TEST_F(IntegerParserTest, FullMessageWithoutSpacesTest)
{
    ASSERT_THAT(
          ParseMsg("-1376\n\r-555\n\r-9\n\r0\n\r1\n\r10\n\r999\n\r1279\n\r120000001\n\r")
        , testing::ElementsAre(-1376, -555, -9 ,0, 1, 10, 999, 1279 ,120000001)
    );
}

TEST_F(IntegerParserTest, FullMessageWithSpacesTest)
{
    ASSERT_THAT(
           ParseMsg(" -1376\n\r-555\n\r  -9\n\r0\n\r    1\n\r10\n\r999\n\r   \t   1279\n\r120000001\n\r  ")
         , testing::ElementsAre(-1376, -555, -9 ,0, 1, 10, 999, 1279 ,120000001)
    );
}

TEST_F(IntegerParserTest, SeparatedMessageWithOneValueWithoutSpacesTest)
{
    ASSERT_THAT(ParseMsg("5"), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\n"), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\r"), testing::ElementsAre(5));

    ASSERT_THAT(ParseMsg("-12345\n\r-1\n\r0\n\r1\n\r12345\n\r"), testing::ElementsAre(-12345, -1, 0, 1, 12345));
}

TEST_F(IntegerParserTest, SeparatedMessageWithOneValueWithSpacesTest)
{
    ASSERT_THAT(ParseMsg("5  "), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\n  \t\t\t"), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\r"), testing::ElementsAre(5));

    ASSERT_THAT(ParseMsg("-12345\n\r-1\n\r0\n\r1\n\r12345\n\r"), testing::ElementsAre(-12345, -1, 0, 1, 12345));
}

TEST_F(IntegerParserTest, SeparatedMessageWithoutSpacesTest)
{
    ASSERT_THAT(ParseMsg("5"), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\n\r-123"), testing::ElementsAre(5));
    ASSERT_THAT(ParseMsg("45\n\r0\n\r1\n\r-2\n\r3"), testing::ElementsAre(-12345, 0, 1, -2));
    ASSERT_THAT(ParseMsg("\n\r"), testing::ElementsAre(3));

    ASSERT_THAT(ParseMsg("-12345\n\r-1\n\r0\n\r1\n\r12345\n\r"), testing::ElementsAre(-12345, -1, 0, 1, 12345));
}

TEST_F(IntegerParserTest, SeparatedMessageWithSpacesTest)
{
    ASSERT_THAT(ParseMsg("5   "), testing::ElementsAre());
    ASSERT_THAT(ParseMsg("\n\r  -123"), testing::ElementsAre(5));
    ASSERT_THAT(ParseMsg("45\n\r0\n\r1\n\r-2\n\r3"), testing::ElementsAre(-12345, 0, 1, -2));
    ASSERT_THAT(ParseMsg("\n\r"), testing::ElementsAre(3));

    ASSERT_THAT(ParseMsg("-12345\n\r-1\n\r0\n\r1\n\r12345\n\r"), testing::ElementsAre(-12345, -1, 0, 1, 12345));
}

TEST_F(IntegerParserTest, InvalidMessageFormatTest)
{
    ASSERT_THROW(ParseMsg("5a\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("5 5\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("0za0\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("5\n\r 1 1\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("\n\n\r\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("100000000000000000000000000000000000000"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("100000000000000000000000000000000000000\n\r"), ParserException);
    Reset();
    ASSERT_THROW(ParseMsg("                                            "), ParserException);
    Reset();
    ASSERT_NO_THROW(ParseMsg(std::string(kBacklogSize - 1, '1')));
    ASSERT_THROW(ParseMsg("\n\r"),ParserException);
}