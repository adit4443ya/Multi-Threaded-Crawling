#include "utils.h"

std::string getDomain(const std::string& url)
{
    size_t start = url.find("://");
    if (start == std::string::npos)
    {
        return "";
    }
    start += 3;
    size_t end = url.find('/', start);
    return url.substr(start, end - start);
}