#pragma once
//
// Created by dmitrii on 2/9/20.
//

#ifndef TIME_INTERVAL_SUM_SERVER_ITIMEPROVIDER_H
#define TIME_INTERVAL_SUM_SERVER_ITIMEPROVIDER_H

class ITimeProvider
{
public:
    virtual ~ITimeProvider() {}
    // Returns number of milliseconds since epoch
    virtual size_t now() = 0;
};
using ITimeProviderPtr = std::shared_ptr<ITimeProvider>;



#endif //TIME_INTERVAL_SUM_SERVER_ITIMEPROVIDER_H
