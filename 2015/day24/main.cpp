#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <numeric>
#include <cstring>
#include <climits>
#include <cmath>
#include <algorithm>
#include <numeric>

/*************
 * Setup code
 **************/

std::string part1(std::stringstream &file_content);
std::string part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream file_content;
    file_content << file.rdbuf();
    file.close();

    return std::move(file_content);
};

void executeFunction(std::stringstream &file_content, std::function<std::string(std::stringstream &)> function, std::string label, std::string expected)
{

    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::string result = function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if (result != expected)
    {
        std::cerr << "\033[1;31m" << label << " failed. Expected " << expected << " but got " << result << "\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[1;32m" << label << " result: " << result << "\033[0m" << std::endl;
    }
    std::cerr << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <path> <expected_path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::string expected_path = argv[2];
    std::optional<std::stringstream> file_content = readFileContent(path);
    std::optional<std::stringstream> file_expected_content = readFileContent(expected_path);

    if (!file_content)
    {
        std::cerr << "Could not read file " << path << std::endl;
        return 1;
    }

    if (!file_expected_content)
    {
        std::cerr << "Could not read file " << expected_path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;
    std::stringstream &file_expected_content_ref = *file_expected_content;

    std::string expected;

    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part1, "Part 1", expected);
    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part2, "Part 2", expected);

    return 0;
}

/***************************************
 *              Solutions
 **************************************/
unsigned long long findMinimumEntanglement(int targetWeight, std::vector<int> &weights, int currentIndex, int currentCount, unsigned long long currentEntanglement, int &minCount, unsigned long long &minEntanglement)
{
    if (targetWeight < 0 || currentCount > minCount || (currentCount == minCount && currentEntanglement >= minEntanglement))
        return minEntanglement;
    if (targetWeight == 0)
    {
        if (currentCount < minCount || (currentCount == minCount && currentEntanglement < minEntanglement))
        {
            minCount = currentCount;
            minEntanglement = currentEntanglement;
        }
        return minEntanglement;
    }

    for (int i = currentIndex; i < static_cast<int>(weights.size()); ++i)
        findMinimumEntanglement(targetWeight - weights[i], weights, i + 1, currentCount + 1, currentEntanglement * weights[i], minCount, minEntanglement);

    return minEntanglement;
}

std::string processFile(std::stringstream &fileContent, int packCount)
{
    std::vector<int> weights;
    while (!fileContent.eof())
    {
        int weight;
        fileContent >> weight;
        weights.push_back(weight);
    }

    int totalWeight = std::accumulate(std::begin(weights), std::end(weights), 0);
    int targetWeight = totalWeight / packCount;

    std::reverse(std::begin(weights), std::end(weights));

    int minCount = INT_MAX;
    unsigned long long minEntanglement = ULLONG_MAX;

    return std::to_string(findMinimumEntanglement(targetWeight, weights, 0, 0, 1, minCount, minEntanglement));
}

std::string part1(std::stringstream &fileContent)
{
    return processFile(fileContent, 3);
}

std::string part2(std::stringstream &fileContent)
{
    return processFile(fileContent, 4);
}