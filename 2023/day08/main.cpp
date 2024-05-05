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
#include <unordered_map>
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

struct Node
{
    std::string key;
    Node *left;
    Node *right;
};

std::vector<std::vector<std::string>> parseInput(std::stringstream &file_content)
{
    std::vector<std::vector<std::string>> parsed_input;
    std::string line;
    while (std::getline(file_content, line))
    {

        std::string currentString = "";
        std::vector<std::string> currentLine;
        for (char c : line)
        {
            if (std::isalnum(c))
            {
                currentString += c;
            }
            else if (currentString != "")
            {
                currentLine.push_back(currentString);
                currentString = "";
            }
        }

        if (currentLine.size() > 0)
        {
            parsed_input.push_back(currentLine);
        }
    }
    return parsed_input;
}

std::string part1(std::stringstream &file_content)
{

    std::string instructions;
    std::getline(file_content, instructions);
    file_content.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto parsed_input = parseInput(file_content);

    std::unordered_map<std::string, Node> nodes;

    for (auto &v : parsed_input)
    {
        std::string &key = v[0];
        std::string &left = v[1];
        std::string &right = v[2];

        Node *keyNode = nullptr;
        Node *leftNode = nullptr;
        Node *rightNode = nullptr;

        if (nodes.find(key) == nodes.end())
        {
            keyNode = &nodes[key];
            keyNode->key = key;
        }

        if (nodes.find(left) == nodes.end())
        {
            leftNode = &nodes[left];
            leftNode->key = left;
        }

        if (nodes.find(right) == nodes.end())
        {
            rightNode = &nodes[right];
            rightNode->key = right;
        }

        keyNode = &nodes[key];
        leftNode = &nodes[left];
        rightNode = &nodes[right];

        keyNode->left = leftNode;
        keyNode->right = rightNode;
    }

    int steps = 0;
    Node *currentNode = &nodes["AAA"];
    size_t instructionLength = instructions.length();
    int instructionIndex = 0;
    while (true)
    {
        char instruction = instructions[instructionIndex++ % instructionLength];
        if (currentNode->key == "ZZZ")
        {
            break;
        }
        if (instruction == 'R')
        {
            currentNode = currentNode->right;
        }
        else if (instruction == 'L')
        {
            currentNode = currentNode->left;
        }
        steps++;
    }

    return std::to_string(steps);
}

std::string part2(std::stringstream &file_content)
{

    std::string instructions;
    std::getline(file_content, instructions);
    file_content.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto parsed_input = parseInput(file_content);

    std::unordered_map<std::string, Node> nodes;

    for (auto &v : parsed_input)
    {
        std::string &key = v[0];
        std::string &left = v[1];
        std::string &right = v[2];

        Node *keyNode = nullptr;
        Node *leftNode = nullptr;
        Node *rightNode = nullptr;

        if (nodes.find(key) == nodes.end())
        {
            keyNode = &nodes[key];
            keyNode->key = key;
        }

        if (nodes.find(left) == nodes.end())
        {
            leftNode = &nodes[left];
            leftNode->key = left;
        }

        if (nodes.find(right) == nodes.end())
        {
            rightNode = &nodes[right];
            rightNode->key = right;
        }

        keyNode = &nodes[key];
        leftNode = &nodes[left];
        rightNode = &nodes[right];

        keyNode->left = leftNode;
        keyNode->right = rightNode;
    }

    std::vector<Node *> current_nodes;
    for (auto &node_pair : nodes)
    {
        if (node_pair.second.key.back() == 'A')
        {
            current_nodes.push_back(&node_pair.second);
        }
    }


    int steps = 0;
    size_t instruction_length = instructions.length();
    int instruction_index = 0;

    std::vector<std::vector<std::tuple<std::string, int, int>>> node_steps(current_nodes.size());
    std::vector<std::vector<std::tuple<std::string, int, int>>> confirmedSteps(current_nodes.size());
    std::vector<int> _nodes_steps_cycle_len(current_nodes.size(), 0);

    while (true)
    {
        bool will_continue = false;
        char instruction = instructions[instruction_index];

        int k = 0;
        for (auto &node : current_nodes)
        {
            if (node->key.back() == 'Z')
            {
                auto tuple = std::make_tuple(node->key, instruction_index, _nodes_steps_cycle_len[k]);
                _nodes_steps_cycle_len[k] = 0;

                if (std::find(node_steps[k].begin(), node_steps[k].end(), tuple) == node_steps[k].end())
                {
                    node_steps[k].push_back(tuple);
                }
            }

            if (node->key.back() != 'Z')
            {
                will_continue = true;
            }

            k++;
        }

        std::unordered_map<int, int> node_step_map;
        for (auto node_step : node_steps)
        {
            for (auto &tuple : node_step)
            {
                auto index = std::get<1>(tuple);
                node_step_map[index]++;
                if (node_step_map[index] == (int)current_nodes.size())
                {
                    unsigned long long lcm = 1;
                    for (auto &node_step : node_steps)
                    {
                        auto it = std::find_if(node_step.begin(), node_step.end(), [index](const std::tuple<std::string, int, int> &tuple) {
                            return std::get<1>(tuple) == index;
                        });
                        auto cycleLen = std::get<2>(*it);
                        lcm = std::lcm(lcm, cycleLen);
                    }

                    return std::to_string(lcm);
                }
                
            }
        }

        if (!will_continue)
        {
            break;
        }

        k = 0;
        for (auto &node : current_nodes)
        {

            if (instruction == 'R')
            {
                node = node->right;
            }
            else if (instruction == 'L')
            {
                node = node->left;
            }

            _nodes_steps_cycle_len[k]++;
            k++;
        }

        steps++;
        instruction_index = (instruction_index + 1) % instruction_length;
    }

    for (auto &node_step : node_steps)
    {
        for (auto &[node, index, steps] : node_step)
        {
            std::cout << node << " " << index << " " << steps << "\n";
        }
        std::cout << "\n";
    }

    return std::to_string(steps);
}
