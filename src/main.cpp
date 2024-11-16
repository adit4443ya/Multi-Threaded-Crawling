#include "web_crawler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <seed_url> <num_threads> <depth_level>" << std::endl;
        return 1;
    }

    std::string seed_url = argv[1];
    int num_threads = std::atoi(argv[2]);
    int depth_level = std::atoi(argv[3]);

    // Basic validation of num_threads and depth_level
    if (num_threads <= 0 || depth_level < 0)
    {
        std::cerr << "Error: Number of threads must be positive and depth level cannot be negative." << std::endl;
        return 1;
    }

    // Child process to run the web crawling
    try
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            // Fork failed
            std::cerr << "Error: Fork failed." << std::endl;
            return 1;
        }
        if (pid == 0)
        {
            WebCrawler crawler(num_threads, depth_level, 1000);
            crawler.start(seed_url);
            crawler.waitForCompletion();
        }
        else
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0); // Wait for the child process to complete
            std::cout << "Crawling completed successfully!" << std::endl;

            if (WIFEXITED(status))
            {
                int exitStatus = WEXITSTATUS(status);
                if (exitStatus == 0)
                {
                    std::cout << "Crawling completed successfully!" << std::endl;
                    
                    // Now, for upload.js (run as a new child process)
                    pid_t uploadPid = fork();
                    if (uploadPid < 0)
                    {
                        std::cerr << "Error: Fork for upload.js failed." << std::endl;
                        return 1;
                    }
                    if (uploadPid == 0)
                    {
                        // Child process runs Node.js to execute upload.js
                        execlp("node", "node", "upload.js", (char *)NULL); // Run upload.js using Node.js
                        std::cerr << "Error: Failed to execute upload.js." << std::endl;
                        exit(1);  // Ensure child process exits on failure
                    }
                    else
                    {
                        // Wait for upload.js to finish
                        waitpid(uploadPid, &status, 0);
                        if (WIFEXITED(status))
                        {
                            std::cout << "Upload.js executed successfully!" << std::endl;
                        }
                        else
                        {
                            std::cout << "Error: upload.js terminated abnormally." << std::endl;
                        }
                    }
                }
                else
                {
                    std::cout << "Crawl process terminated with error." << std::endl;
                }
            }
            else
            {
                std::cout << "Crawl process terminated abnormally." << std::endl;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in child process: " << e.what() << std::endl;
        return 1;
    }

    return 0; // Parent process returns
}
