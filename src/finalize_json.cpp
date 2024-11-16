#include "finalize_json.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void finalizeJsonFiles(const std::string &outputDir)
{
    // Get the current working directory (root of your project)
    fs::path cwd = fs::current_path();  // Current working directory
    fs::path fullOutputPath = cwd / outputDir;  // Combine with 'crawler_output' directory
    
    std::cout << "Working directory: " << cwd << std::endl;
    std::cout << "Full output directory path: " << fullOutputPath << std::endl;

    // Iterate through all files in the specified directory
    for (const auto &entry : fs::directory_iterator(fullOutputPath))
    {
        if (fs::is_regular_file(entry))
        {
            std::ifstream file(entry.path());
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

            // Check if the file is not empty
            if (!content.empty())
            {
                // Check if the file already ends with "]" (indicating it's a valid JSON array)
                if (content.back() != ']')
                {
                    // If not, add the closing bracket to complete the JSON array
                    std::ofstream outFile(entry.path(), std::ios::app);
                    outFile << "\n]";
                }
            }
            else
            {
                // If the file is empty, write an empty JSON array: "[]"
                std::ofstream outFile(entry.path());
                outFile << "[]";
            }
        }
    }
}
