#pragma once

#include <string>
#include <vector>

struct PageMetadata
{
    std::string url;
    std::string title;
    std::string description;
    std::vector<std::string> links;
    int depth;
};