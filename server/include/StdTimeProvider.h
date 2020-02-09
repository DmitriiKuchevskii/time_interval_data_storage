#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_TIME_H
#define TIME_INTERVAL_SUM_SERVER_TIME_H

#include <cstddef>
#include <memory>
#include <chrono>

#include "ITimeProvider.h"

class StdTimeProvider : public ITimeProvider
{
public:
    size_t now() override
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }
};

#endif //TIME_INTERVAL_SUM_SERVER_TIME_H
