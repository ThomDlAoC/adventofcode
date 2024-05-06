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

enum class Order
{
    OFF,
    ON,
    TOGGLE
};

using loc_t = std::pair<int, int>;
using order_t = std::tuple<Order, loc_t, loc_t>;

loc_t parseLoc(std::string &str)
{
    std::stringstream ss(str);
    std::string current_chunk;
    std::vector<std::string> chunks;
    loc_t current_loc;
    for (; std::getline(ss, current_chunk, ',');)    
        chunks.push_back(current_chunk);
    
    current_loc.first = std::stoi(chunks[0]);
    current_loc.second = std::stoi(chunks[1]);

    return current_loc;
}

order_t parseOrder(std::string &str)
{
    std::stringstream ss(str);
    std::string current_chunk;
    loc_t in, out;
    Order current_order;

    ss >> current_chunk;
    if (current_chunk == "turn")
    {
        ss >> current_chunk;
        if (current_chunk == "on")        
            current_order = Order::ON;        
        else        
            current_order = Order::OFF;
        
    }
    else
    {
        current_order = Order::TOGGLE;
    }

    ss >> current_chunk;
    in = parseLoc(current_chunk);

    ss >> current_chunk;
    ss >> current_chunk;
    out = parseLoc(current_chunk);

    return std::make_tuple(current_order, in, out);
}



std::string part1(std::stringstream &file_content)
{
    std::vector<order_t> orders;

    for (std::string line; std::getline(file_content, line);)
        orders.push_back(parseOrder(line));

    std::array<std::array<bool, 1000>, 1000> grid{};

    for (const auto &order : orders)
    {
        const auto &[current_order, in, out] = order;
        const auto &[x_in, y_in] = in;
        const auto &[x_out, y_out] = out;

        for (int x = x_in; x <= x_out; x++)
        {
            for (int y = y_in; y <= y_out; y++)
            {
                if (current_order == Order::ON)
                    grid[x][y] = true;
                else if (current_order == Order::OFF)
                    grid[x][y] = false;
                else if (current_order == Order::TOGGLE)
                    grid[x][y] = !grid[x][y];
            }
        }
    }

    int total = 0;

    for (int x = 0; x < 1000; x++)
        for (int y = 0; y < 1000; y++)
            if (grid[x][y])
                total++;

    return std::to_string(total);
}

std::string part2(std::stringstream &file_content)
{
    std::vector<order_t> orders;

    for (std::string line; std::getline(file_content, line);)
        orders.push_back(parseOrder(line));

    std::array<std::array<int, 1000>, 1000> grid{};

    for (const auto &order : orders)
    {
        const auto &[current_order, in, out] = order;
        const auto &[x_in, y_in] = in;
        const auto &[x_out, y_out] = out;

        for (int x = x_in; x <= x_out; x++)
        {
            for (int y = y_in; y <= y_out; y++)
            {
                if (current_order == Order::ON)
                    grid[x][y]++;
                else if (current_order == Order::OFF)
                    grid[x][y] = std::max(0, grid[x][y] - 1);
                else if (current_order == Order::TOGGLE)
                    grid[x][y] += 2;
            }
        }
    }

    int total = 0;

    for (int x = 0; x < 1000; x++)
        for (int y = 0; y < 1000; y++)
            total += grid[x][y];

    return std::to_string(total);
}
