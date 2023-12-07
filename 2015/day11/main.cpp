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

/*************
 * Setup code
 **************/

std::string part1(std::stringstream &file_content);
std::string part2(std::stringstream &file_content);

void incrementWord(std::__cxx11::string &word);

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

bool isWordValid(std::string word)
{

    bool hasStraight = false;
    int twoPairCount = 0;
    char lastChar = 0;
    char prevChar = 0;
    for (char c : word)
    {
        if (c == 'i' || c == 'o' || c == 'l')
            return false;
        if (c == lastChar && c != prevChar)
            twoPairCount++;
        if (c == lastChar + 1 && c == prevChar + 2)
            hasStraight = true;
        prevChar = lastChar;
        lastChar = c;
    }

    return hasStraight && (twoPairCount >= 2);
}

void incrementWord(std::string &word)
{
    for (int i = word.size() - 1; i >= 0; i--)
    {
        if (word[i] == 'z')
        {
            word[i] = 'a';
        }
        else
        {
            word[i]++;
            break;
        }
    }
}

// Brute force solution

std::string part1(std::stringstream &file_content)
{

    std::string word;
    file_content >> word;

    while (!isWordValid(word))
        incrementWord(word);

    return word;
}

std::string part2(std::stringstream &file_content)
{

    std::string word;
    file_content >> word;

    for (int i = 2; i; --i)
    {
        incrementWord(word);
        while (!isWordValid(word))
            incrementWord(word);
    }

    return word;
}
