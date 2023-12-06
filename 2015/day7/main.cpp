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

bool isNumber(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c)
                                      { return !std::isdigit(c); }) == s.end();
}

enum class OPERATION
{
    VAL,
    TRANSFERT,
    AND,
    OR,
    LSHIFT,
    RSHIFT,
    NOT
};

struct Operation
{
    OPERATION operation;
    std::string input1;
    std::string input2;
    std::string output;
};

std::vector<Operation> parseOperations(std::stringstream &file_content)
{
    std::vector<Operation> operations;
    std::string line;
    while (std::getline(file_content, line))
    {

        std::string input = line.substr(0, line.find("->"));
        std::stringstream output_stream(line.substr(line.find("->") + 2));
        std::string output;
        output_stream >> output;

        std::stringstream input_stream(input);
        std::string content;
        std::vector<std::string> input_parts;
        while (input_stream >> content)
        {
            input_parts.push_back(content);
        }

        if (input_parts.size() == 1)
        {

            if (isNumber(input_parts[0]))
            {
                Operation operation;
                operation.operation = OPERATION::VAL;
                operation.input1 = input_parts[0];
                operation.output = output;
                operations.push_back(operation);
            }
            else
            {
                Operation operation;
                operation.operation = OPERATION::TRANSFERT;
                operation.input1 = input_parts[0];
                operation.output = output;
                operations.push_back(operation);
            }
        }
        else if (input_parts.size() == 2)
        {
            Operation operation;
            operation.operation = OPERATION::NOT;
            operation.input1 = input_parts[1];
            operation.output = output;
            operations.push_back(operation);
        }
        else if (input_parts.size() == 3)
        {
            Operation operation;
            operation.output = output;
            if (input_parts[1] == "AND")
            {
                operation.operation = OPERATION::AND;
            }
            else if (input_parts[1] == "OR")
            {
                operation.operation = OPERATION::OR;
            }
            else if (input_parts[1] == "LSHIFT")
            {
                operation.operation = OPERATION::LSHIFT;
            }
            else if (input_parts[1] == "RSHIFT")
            {
                operation.operation = OPERATION::RSHIFT;
            }
            operation.input1 = input_parts[0];
            operation.input2 = input_parts[2];
            operations.push_back(operation);
        }
        else
        {
            std::cout << "Invalid operation: " << line << std::endl;
        }
    }
    return operations;
}

std::string evaluate(std::unordered_map<std::string, Operation *> &operations_map, const std::string &ending, std::unordered_map<std::string, std::string> &cache)
{

    if (cache.find(ending) != cache.end())
    {
        return cache[ending];
    }

    if (isNumber(ending))
    {
        return ending;
    }
    Operation *operation = operations_map[ending];
    if (operation->operation == OPERATION::VAL)
    {
        return cache[ending] = operation->input1;
    }
    else if (operation->operation == OPERATION::TRANSFERT)
    {
        return cache[ending] = evaluate(operations_map, operation->input1, cache);
    }
    else if (operation->operation == OPERATION::NOT)
    {

        return cache[ending] = std::to_string(~std::stoi(evaluate(operations_map, operation->input1, cache)));
    }
    else if (operation->operation == OPERATION::AND)
    {
        return cache[ending] = std::to_string(std::stoi(evaluate(operations_map, operation->input1, cache)) & std::stoi(evaluate(operations_map, operation->input2, cache)));
    }
    else if (operation->operation == OPERATION::OR)
    {
        return cache[ending] = std::to_string(std::stoi(evaluate(operations_map, operation->input1, cache)) | std::stoi(evaluate(operations_map, operation->input2, cache)));
    }
    else if (operation->operation == OPERATION::LSHIFT)
    {
        return cache[ending] = std::to_string(std::stoi(evaluate(operations_map, operation->input1, cache)) << std::stoi(evaluate(operations_map, operation->input2, cache)));
    }
    else if (operation->operation == OPERATION::RSHIFT)
    {
        return cache[ending] = std::to_string(std::stoi(evaluate(operations_map, operation->input1, cache)) >> std::stoi(evaluate(operations_map, operation->input2, cache)));
    }
    else
    {
        return "";
    }
}

void part1(std::stringstream &file_content)
{

    auto operations = parseOperations(file_content);

    std::unordered_map<std::string, Operation *> operations_map;

    for (auto &operation : operations)
    {
        operations_map[operation.output] = &operation;
    }

    std::unordered_map<std::string, std::string> cache;
    std::string res = evaluate(operations_map, "a", cache);

    std::cout << "Result: " << res << std::endl;
}

void part2(std::stringstream &file_content)
{

    auto operations = parseOperations(file_content);

    std::unordered_map<std::string, Operation *> operations_map;

    for (auto &operation : operations)
    {
        operations_map[operation.output] = &operation;
    }

    std::unordered_map<std::string, std::string> cache;
    std::string res = evaluate(operations_map, "a", cache);

    cache.clear();

    Operation newop = {
        OPERATION::VAL,
        res,
        "",
        "b"};

    operations_map["b"] = &newop;

    res = evaluate(operations_map, "a", cache);

    std::cout << "Result: " << res << std::endl;
}
