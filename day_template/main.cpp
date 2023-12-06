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

void part1(std::stringstream &file_content);
void part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Could not open file " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream file_content;
    file_content << file.rdbuf();
    file.close();

    return std::move(file_content);
};

void executeFunction(std::stringstream &file_content, std::function<void(std::stringstream &)> function)
{
    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::optional<std::stringstream> file_content = readFileContent(path);

    if (!file_content)
    {
        std::cout << "Could not read file " << path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;

    executeFunction(file_content_ref, part1);
    executeFunction(file_content_ref, part2);

    return 0;
}

/***************************************
 *              Solutions
 **************************************/


void part1(std::stringstream &file_content)
{

}

void part2(std::stringstream &file_content)
{
    
}
