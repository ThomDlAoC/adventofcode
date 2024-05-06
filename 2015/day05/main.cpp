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
#include <unordered_set>
#include <array>

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
std::unordered_set<char> vowels = {'a', 'e', 'i', 'o', 'u'};

bool isNicePart1(std::string &input_string){

    int vowel_count = 0;
    bool has_twice = false;
    char previous = 0;
    for (char current_char : input_string)
    {
        if (vowels.contains(current_char))
            vowel_count++;
        if (previous == current_char)
            has_twice = true;
        std::string pair = std::string(1, previous) + std::string(1, current_char);
        if (pair == "ab" || pair == "cd" || pair == "pq" || pair == "xy")
            return false;
        previous = current_char;
    }
    return vowel_count >= 3 && has_twice;
}

bool isNicePart2(std::string &input_string){
    
    std::array previous_chars = {'\0','\0'};
    bool is_repeating=false;
    bool is_repeating_twice=false;
    int current_index = 0;
    for (char current_char : input_string)
    {
        std::string pair = std::string(1, previous_chars[0]) + std::string(1, current_char);
        std::size_t found = input_string.find(pair, current_index + 1);
        if (found != std::string::npos)
            is_repeating_twice = true;

        if (previous_chars[1] == current_char)
            is_repeating = true;

        if (is_repeating && is_repeating_twice)
           return true;

        previous_chars[1] = previous_chars[0];
        previous_chars[0] = current_char;
        current_index++;
    }

    return false;
}

std::string part1(std::stringstream &file_content)
{
    std::string line;
    int total=0;
    
    for(;std::getline(file_content,line);)    
        total += isNicePart1(line);

    return std::to_string(total);

}

std::string part2(std::stringstream &file_content)
{
    std::string line;
    int total=0;
    
    for(;std::getline(file_content,line);)    
        total += isNicePart2(line);
        
    return std::to_string(total);
}
