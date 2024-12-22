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

uint32_t mix(uint32_t secret, uint32_t value)
{
    return secret ^ value;
}

uint32_t prune(uint32_t secret)
{
    return secret % 16777216;
}

uint32_t calcNextSecret(uint32_t secret)
{
    secret = prune(mix(secret, secret << 6));
    secret = prune(mix(secret, secret >> 5));
    secret = prune(mix(secret, secret << 11));
    return secret;
}

std::string part1(std::stringstream &file_content)
{

    uint64_t sum_secret = 0;

    for (std::string line; std::getline(file_content, line);)
    {
        uint32_t secret = std::stoul(line);
        for (int i = 0; i < 2000; i++)
            secret = calcNextSecret(secret);

        sum_secret += secret;
    }

    return std::to_string(sum_secret);
}

std::string part2(std::stringstream &file_content)
{
    std::unordered_map<uint32_t, int32_t> seqmap;
    std::unordered_set<uint32_t> seen;
    int32_t maxval = INT_MIN;

    for (std::string line; std::getline(file_content, line);)
    {
        int32_t secret = std::stoi(line);
        int32_t prev = secret % 10;
        std::array<int32_t, 4> seq;
        seen.clear();

        for (int i = 0; i < 2000; i++)
        {
            secret = calcNextSecret(secret);
            uint32_t curr = secret % 10;
            seq[i % 4] = curr - prev;
            prev = curr;
            if (i > 3)
            {
                uint32_t seqkey = 0;
                int firstidx = (4 + (i % 4) - 3) % 4;

                for (int j = 0; j < 4; j++)
                    seqkey = (seqkey << 8) | (seq[(firstidx + j) % 4] & 0xFF);

                if (seen.count(seqkey))
                    continue;
                seen.insert(seqkey);
                seqmap[seqkey] += curr;
                maxval = std::max(maxval, seqmap[seqkey]);
            }
        }
    }

    return std::to_string(maxval);
}
