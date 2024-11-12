#include "web_crawler.h"
#include "utils.h"
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "json.hpp"
#include <filesystem>
#include "thread_safe_queue.h"

using namespace std::chrono_literals;
using json = nlohmann::json;
namespace fs = std::filesystem;

size_t WebCrawler::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
    userp->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

std::vector<std::string> WebCrawler::extractLinks(const std::string& html, const std::string& baseUrl)
{
    std::vector<std::string> links;
    GumboOutput* output = gumbo_parse(html.c_str());
    extractLinksHelper(output->root, links, baseUrl);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}

void WebCrawler::extractLinksHelper(GumboNode* node, std::vector<std::string>& links, const std::string& baseUrl)
{
    // Implementation
    if (node->type != GUMBO_NODE_ELEMENT)
        {
            return;
        }

        if (node->v.element.tag == GUMBO_TAG_A)
        {
            GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
            if (href)
            {
                std::string url = href->value;
                if (!url.empty() && url[0] != '#')
                {
                    if (url.find("http") != 0)
                    {
                        url = baseUrl + (url[0] == '/' ? "" : "/") + url;
                    }
                    links.push_back(url);
                }
            }
        }

        GumboVector *children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            extractLinksHelper(static_cast<GumboNode *>(children->data[i]), links, baseUrl);
        }
}


PageMetadata WebCrawler::extractMetadata(const std::string& html, const std::string& url, int depth)
{
    // Implementation
    PageMetadata metadata;
        metadata.url = url;
        metadata.depth = depth;

        GumboOutput *output = gumbo_parse(html.c_str());
        GumboNode *root = output->root;

        // Search for the title tag
        GumboNode *title_node = nullptr;
        GumboVector *head_children = &root->v.element.children;
        for (unsigned int i = 0; i < head_children->length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(head_children->data[i]);
            if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_TITLE)
            {
                title_node = child;
                break;
            }
        }

        // Extract title text if the title tag is found
        if (title_node && title_node->v.element.children.length > 0)
        {
            GumboNode *title_text = static_cast<GumboNode *>(title_node->v.element.children.data[0]);
            metadata.title = title_text->v.text.text;
        }
        else
        {
            metadata.title = "No title";
        }

        // Extract meta description
        metadata.description = "No description";
        for (unsigned int i = 0; i < head_children->length; ++i)
        {
            GumboNode *child = static_cast<GumboNode *>(head_children->data[i]);
            if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_META)
            {
                GumboAttribute *name = gumbo_get_attribute(&child->v.element.attributes, "name");
                if (name && strcmp(name->value, "description") == 0)
                {
                    GumboAttribute *content = gumbo_get_attribute(&child->v.element.attributes, "content");
                    if (content)
                    {
                        metadata.description = content->value;
                        break;
                    }
                }
            }
        }

        // Extract links
        metadata.links = extractLinks(html, url);

        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return metadata;
}

void WebCrawler::processUrl(const std::string& url, int depth, int threadId)
{
    // Implementation
    if (depth > maxDepth)
            return;

        // Implement rate limiting
        std::string domain = getDomain(url);
        std::chrono::steady_clock::time_point lastRequest;
        {
            std::lock_guard<std::mutex> lock(visitedMutex);
            auto it = domainLastRequest.find(domain);
            if (it != domainLastRequest.end())
            {
                lastRequest = it->second;
            }
            else
            {
                lastRequest = std::chrono::steady_clock::now();
                domainLastRequest[domain] = lastRequest;
            }
        }

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds delayRemaining = domainDelay - std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRequest);
        if (delayRemaining.count() > 0)
        {
            std::this_thread::sleep_for(delayRemaining);
        }

        CURL *curl = curl_easy_init();
        if (!curl)
            return;

        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            PageMetadata metadata = extractMetadata(response_string, url, depth);

            // Write metadata to thread-specific file
            json j;
            j["url"] = metadata.url;
            j["title"] = metadata.title;
            j["description"] = metadata.description;
            j["depth"] = metadata.depth;
            j["links"] = metadata.links;

            {
                std::lock_guard<std::mutex> lock(filesMutex);
                threadFiles[threadId] << j.dump(4) << std::endl;
                threadFiles[threadId].flush();
            }

            // Add new URLs to queue
            for (const auto &link : metadata.links)
            {
                bool shouldAdd = false;
                {
                    std::lock_guard<std::mutex> lock(visitedMutex);
                    if (visitedUrls.find(link) == visitedUrls.end())
                    {
                        visitedUrls.insert(link);
                        shouldAdd = true;
                    }
                }
                if (shouldAdd)
                {
                    urlQueue.push(link);
                }
            }
        }

        curl_easy_cleanup(curl);

        // Update last request time for the domain
        {
            std::lock_guard<std::mutex> lock(visitedMutex);
            domainLastRequest[domain] = std::chrono::steady_clock::now();
        }
}

void WebCrawler::workerThread(int threadId)
{
    // Implementation
    activeThreads++;
        while (!shouldStop)
        {
            std::string url;
            if (urlQueue.try_pop(url))
            {
                processUrl(url, 0, threadId);
            }
            else
            {
                std::this_thread::sleep_for(100ms);
                if (activeThreads == 1 && urlQueue.empty())
                {
                    shouldStop = true;
                }
            }
        }
        activeThreads--;
}

WebCrawler::WebCrawler(int threads, int depth, int delayMs)
    : maxDepth(depth), numThreads(threads), domainDelay(delayMs)
{
    curl_global_init(CURL_GLOBAL_ALL);
    fs::create_directory("crawler_output");

    for (int i = 0; i < numThreads; i++)
    {
        std::string filename = "crawler_output/thread_" + std::to_string(i) + ".json";
        threadFiles.emplace_back(filename);
    }
}

WebCrawler::~WebCrawler()
{
    stop();
    curl_global_cleanup();
    for (auto& file : threadFiles)
    {
        file.close();
    }
}

void WebCrawler::start(const std::string& seedUrl)
{
    // Implementation
    {
            std::lock_guard<std::mutex> lock(visitedMutex);
            visitedUrls.insert(seedUrl);
        }
        urlQueue.push(seedUrl);

        // Start worker threads
        for (int i = 0; i < numThreads; i++)
        {
            workers.emplace_back(&WebCrawler::workerThread, this, i);
        }
}

void WebCrawler::stop()
{
    // Implementation
    shouldStop = true;
        for (auto &worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
        workers.clear();
}

void WebCrawler::waitForCompletion()
{
    // Implementation
    while (activeThreads > 0 || !urlQueue.empty())
        {
            std::this_thread::sleep_for(100ms);
        }
        stop();
}