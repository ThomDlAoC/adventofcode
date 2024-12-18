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

std::tuple<uint64_t, uint64_t, uint64_t, std::vector<int>> parseInput(std::stringstream &file_content)
{

    uint64_t A, B, C;
    std::vector<int> Prog;
    int line_idx = 0;
    for (std::string line; std::getline(file_content, line); line_idx++)
    {
        if (line_idx == 0)
        {
            sscanf(line.c_str(), "Register A: %lu", &A);
        }
        else if (line_idx == 1)
        {
            sscanf(line.c_str(), "Register B: %lu", &B);
        }
        else if (line_idx == 2)
        {
            sscanf(line.c_str(), "Register C: %lu", &C);
        }
        else if (line_idx == 4)
        {

            auto linum = line.find(": ");
            auto linee = line.substr(linum + 2);
            std::stringstream ss(linee);
            std::string token;
            while (std::getline(ss, token, ','))
            {
                Prog.push_back(std::stoi(token));
            }
        }
    }

    return {A, B, C, Prog};
}

int singleRound(uint64_t A, const std::vector<int> &Prog)
{
    uint64_t B = 0, C = 0;
    int out = 0;
    auto comboOp = [&](int value) -> uint64_t
    {
        if (value >= 0 && value <= 3)
            return (uint64_t)value;
        if (value == 4)
            return A;
        if (value == 5)
            return B;
        if (value == 6)
            return C;
        return 0lu;
    };

    for (int prog_ptr = 0; prog_ptr < (int)Prog.size(); prog_ptr += 2)
    {
        auto op = Prog[prog_ptr];
        auto value = Prog[prog_ptr + 1];
        switch (op)
        {
        case 0:
            A >>= comboOp(value);
            break;
        case 1:
            B ^= value;
            break;
        case 2:
            B = comboOp(value) & 0x7;
            break;
        case 3:
            break;
        case 4:
            B ^= C;
            break;
        case 5:
            out = comboOp(value) & 0x7;
            break;
        case 6:
            B = A >> comboOp(value);
            break;
        case 7:
            C = A >> comboOp(value);
            break;
        }
    }

    return out;
}

std::string getProgOutput(uint64_t A, uint64_t B, uint64_t C, const std::vector<int> &Prog)
{
    std::vector<int> outs{};
    while (A)
    {
        auto b = singleRound(A, Prog);
        outs.push_back(b);
        A >>= 3;
    }
    return std::accumulate(outs.begin() + 1, outs.end(), std::to_string(outs[0]), [](std::string a, int b)
                           { return a + "," + std::to_string(b); });
}

std::string part1(std::stringstream &file_content)
{

    auto [A, B, C, Prog] = parseInput(file_content);
    return getProgOutput(A, B, C, Prog);
}

uint64_t dfs(const std::vector<int> &Prog, uint64_t acc, int idx)
{
    if (idx == (int)Prog.size())
        return acc >> 3;

    for (int i = 0; i < 8; ++i)
    {
        auto Pi = singleRound(acc + i, Prog);
        if (Pi != Prog[Prog.size() - idx - 1])
            continue;

        auto res = dfs(Prog, (acc + i) << 3, idx + 1);
        if (res != UINT64_MAX)
            return res;
    }

    return UINT64_MAX;
}

std::string part2(std::stringstream &file_content)
{
    auto [A, B, C, Prog] = parseInput(file_content);
    return std::to_string(dfs(Prog, 0, 0));
}
