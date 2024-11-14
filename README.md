
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
- [Getting Started](#contributing-guide)

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

### 3. Run the Project

Create a `obj` directory, generate the necessary files with `make`, and compile the project:

```bash
./make.sh
```

---

### 4. Run the Web Crawler

To start crawling from a seed URL, thread, Max Depth, run:

```bash
./bin/web_crawler javatpoint.com 16 3
```

Replace `https://www.javatpoint.com` with the desired seed URL.

---

### Contributing Guide (Quick Commands)

```bash
# 1. Clone your forked repo
git clone https://github.com/your-username/repo-name.git
cd repo-name

# 2. Add upstream remote to keep up with the original repo
git remote add upstream https://github.com/original-owner/repo-name.git
git fetch upstream

# 3. Create a new branch for your feature/fix
git checkout -b feature-or-fix-description

# 4. Before making changes, ensure your local main branch is up-to-date
git checkout main
git fetch upstream
git rebase upstream/main

# 5. Rebase your feature branch onto the updated main branch
git checkout feature-or-fix-description
git rebase main

# 6. Make your changes, then stage and commit with a descriptive message
git add .
git commit -m "Description of changes"

# 7. Push your branch to your fork
git push origin feature-or-fix-description

# 8. If updates happen on upstream/main while your PR is open, keep your branch updated:
git fetch upstream                # Get latest updates from the original repo
git rebase upstream/main           # Rebase your feature branch onto it
git push origin feature-or-fix-description --force  # Force-push updated branch

```

## Conclusion

This web crawler project is a multi-threaded application that effectively crawls websites, extracts relevant metadata, and handles rate-limiting for domains. With its modular design, it is easy to extend or modify the functionality for additional use cases. You can adjust the number of worker threads, crawl depth, and domain request delay for optimized performance based on your requirements.

---

Feel free to contribute to the project by submitting issues or pull requests. Happy crawling!