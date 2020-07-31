#pragma once

struct timestamp
{
    int value;
};

struct timestamp_percent : timestamp
{
    int set_milliseconds;
};