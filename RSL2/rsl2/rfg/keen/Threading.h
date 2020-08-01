#pragma once

namespace keen
{
    struct Mutex
    {
        char m_name[32];
        unsigned int m_criticalSectionData[6];
    };
}

struct cs_wrapper
{
    keen::Mutex m_mutex;
    int debug_handle;
};

struct cs_spinlock
{
    unsigned __int64 lockValue;
};