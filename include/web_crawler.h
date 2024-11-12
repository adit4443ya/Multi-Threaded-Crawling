#pragma once

#include <string>
#include <vector>
#include <thread>
#include <bits/stdc++.h>
#include <atomic>
#include <fstream>
#include "thread_safe_queue.h"
#include "page_metadata.h"
#include <gumbo.h>

class WebCrawler
{
private:
    ThreadSafeQueue<std::string> urlQueue;
    std::unordered_set<std::string> visitedUrls;
    std::mutex visitedMutex;
    std::vector<std::thread> workers;
    std::atomic<bool> shouldStop{false};
    int maxDepth;
    int numThreads;
    std::vector<std::ofstream> threadFiles;
    std::mutex filesMutex;
    std::atomic<int> activeThreads{0};
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> domainLastRequest;
    std::chrono::milliseconds domainDelay{1000};

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    std::vector<std::string> extractLinks(const std::string& html, const std::string& baseUrl);
    void extractLinksHelper(GumboNode* node, std::vector<std::string>& links, const std::string& baseUrl);
    PageMetadata extractMetadata(const std::string& html, const std::string& url, int depth);
    void processUrl(const std::string& url, int depth, int threadId);
    void workerThread(int threadId);

public:
    WebCrawler(int threads = 4, int depth = 3, int delayMs = 1000);
    ~WebCrawler();
    void start(const std::string& seedUrl);
    void stop();
    void waitForCompletion();
};