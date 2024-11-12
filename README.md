
---

# Web Crawler

This project is a multi-threaded web crawler built in C++. It uses the `Curl` library for making HTTP requests, the `Gumbo` library for parsing HTML, and the `nlohmann/json` library for JSON manipulation. The crawler can crawl websites up to a specified depth and save metadata such as page titles, descriptions, and links.

---

## Table of Contents

- [Setup Instructions](#setup-instructions)
- [Installation Guide for Dependencies](#installation-guide-for-dependencies)
  - [Curl Library](#curl-library)
  - [Gumbo Library](#gumbo-library)
  - [nlohmann/json Library](#nlohmannjson-library)
- [Cloning the Repository](#cloning-the-repository)
- [Building the Project](#building-the-project)
- [Running the Web Crawler](#running-the-web-crawler)
- [Header File Documentation](#header-file-documentation)
  - [`thread_safe_queue.h`](#thread_safe_queueh)
  - [`page_metadata.h`](#page_metadath)
  - [`utils.h`](#utilsh)
  - [`web_crawler.h`](#web_crawlerh)

---

## Setup Instructions

To set up and run the web crawler project, follow these instructions:

### 1. Install Dependencies

Make sure you have the following dependencies installed on your system:

- **C++ compiler** (e.g., GCC, Clang)
- **CMake** (version 3.14 or later)
- **Curl library**
- **Gumbo library**
- **nlohmann/json library**

---

## Installation Guide for Dependencies

### 1. Curl Library

#### On Linux:

To install the Curl library, use your package manager. For example, on Debian-based systems (like Ubuntu), run:

```bash
sudo apt-get install libcurl4-openssl-dev
```

#### On Windows:

You can install Curl using [vcpkg](https://github.com/microsoft/vcpkg). First, install `vcpkg` and then run:

```bash
vcpkg install curl
```

### 2. Gumbo Library

#### On Linux:

Install the Gumbo library using the following command:

```bash
sudo apt-get install libgumbo-dev
```

#### On Windows:

To install Gumbo on Windows, use `vcpkg`:

```bash
vcpkg install gumbo-parser
```

### 3. nlohmann/json Library

#### On Linux:

You can install the `nlohmann/json` library via a package manager:

```bash
sudo apt-get install nlohmann-json3-dev
```

#### On Windows:

Install the `nlohmann/json` library using `vcpkg`:

```bash
vcpkg install nlohmann-json
```

Make sure to configure your build system to include the paths to these libraries as needed.

---

### 2. Clone the Repository

```bash
git clone https://github.com/your-username/web-crawler.git
cd web-crawler
```

---

### 3. Build the Project

Create a `build` directory, generate the necessary files with `cmake`, and compile the project:

```bash
mkdir build
cd build
cmake ..
make
```

---

### 4. Run the Web Crawler

To start crawling from a seed URL, run:

```bash
./web_crawler https://www.example.com
```

Replace `https://www.example.com` with the desired seed URL.

---

## Header File Documentation

This section provides detailed documentation for the main header files used in the project.

### `thread_safe_queue.h`

This header file defines a thread-safe implementation of a queue data structure.

#### `ThreadSafeQueue` Class

- **`push(T value)`**: Adds a new element to the end of the queue.
- **`try_pop(T& value)`**: Removes and returns the element at the front of the queue. Returns `false` if the queue is empty.
- **`empty() const`**: Returns `true` if the queue is empty, `false` otherwise.
- **`size() const`**: Returns the number of elements in the queue.

The class uses `std::mutex` and `std::condition_variable` to ensure thread safety.

---

### `page_metadata.h`

This header file defines the `PageMetadata` struct, which holds information about a crawled web page.

#### `PageMetadata` Struct

- **`url`**: The URL of the web page.
- **`title`**: The title of the web page.
- **`description`**: The meta description of the web page.
- **`links`**: A vector of URLs that are links on the web page.
- **`depth`**: The depth of the web page in the crawl hierarchy.

---

### `utils.h`

This header file declares a utility function for extracting the domain from a given URL.

#### `getDomain(const std::string& url)`

- **Input**: A URL as a string (e.g., `https://www.example.com/path/to/page.html`).
- **Output**: The domain portion of the URL (e.g., `example.com`).

---

### `web_crawler.h`

This header file defines the `WebCrawler` class, which is the main component of the web crawler application.

#### `WebCrawler` Class

##### Private Members:

- **`urlQueue`**: A `ThreadSafeQueue` that holds the URLs to be crawled.
- **`visitedUrls`**: An `unordered_set` that tracks visited URLs.
- **`visitedMutex`**: A `mutex` for protecting the `visitedUrls` set from race conditions.
- **`workers`**: A vector of `std::thread` objects representing worker threads.
- **`shouldStop`**: An `std::atomic<bool>` flag used to signal worker threads to stop.
- **`maxDepth`**: The maximum crawl depth.
- **`numThreads`**: The number of worker threads to use.
- **`threadFiles`**: A vector of `std::ofstream` objects for writing crawled data.
- **`filesMutex`**: A `mutex` to protect the `threadFiles` vector from race conditions.
- **`activeThreads`**: An `std::atomic<int>` to track active worker threads.
- **`domainLastRequest`**: An `unordered_map` for rate limiting based on the last request time for each domain.
- **`domainDelay`**: The delay (in milliseconds) between requests to the same domain.

##### Public Methods:

- **`WebCrawler(int threads = 4, int depth = 3, int delayMs = 1000)`**: Constructor that initializes the crawler with the specified number of threads, maximum depth, and domain delay.
- **`~WebCrawler()`**: Destructor that stops the crawler and cleans up resources.
- **`start(const std::string& seedUrl)`**: Starts the crawling process from the specified seed URL.
- **`stop()`**: Stops the crawling process.
- **`waitForCompletion()`**: Waits for the crawling process to complete.

---

## Conclusion

This web crawler project is a multi-threaded application that effectively crawls websites, extracts relevant metadata, and handles rate-limiting for domains. With its modular design, it is easy to extend or modify the functionality for additional use cases. You can adjust the number of worker threads, crawl depth, and domain request delay for optimized performance based on your requirements.

---

Feel free to contribute to the project by submitting issues or pull requests. Happy crawling!