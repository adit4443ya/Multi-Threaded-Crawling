#include "web_crawler.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <seed_url>" << std::endl;
        return 1;
    }

    try
    {
        WebCrawler crawler(4, 3, 1000);
        crawler.start(argv[1]);
        crawler.waitForCompletion();
        std::cout << "Crawling completed successfully!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}