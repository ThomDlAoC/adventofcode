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
#include <unordered_map>
#include <queue>

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

#define TYPE_FLIPFLIP_TOKEN '%'
#define TYPE_CONJUCTOR_TOKEN '&'

enum class ModuleType
{
    None = 0,
    FlipFlop = 1,
    Conjuctor = 2,
};

struct ModuleConfiguration
{
    ModuleType type;
    std::string module_name;
    bool state;
    std::vector<std::string> outputs;
    std::unordered_map<std::string, bool> input_pulse_values;
};

std::unordered_map<std::string, ModuleConfiguration> parseConfigurations(std::stringstream &file_content)
{
    std::unordered_map<std::string, ModuleConfiguration> configuration;
    std::string line;
    while (std::getline(file_content, line))
    {
        if (line.empty())
        {
            break;
        }

        std::stringstream line_stream(line);
        std::string input;
        std::string arrow;
        std::string output;
        line_stream >> input >> arrow;

        ModuleConfiguration module_configuration;
        module_configuration.state = false;
        if (input[0] == TYPE_FLIPFLIP_TOKEN)
        {
            module_configuration.module_name = input.substr(1);
            module_configuration.type = ModuleType::FlipFlop;
        }
        else if (input[0] == TYPE_CONJUCTOR_TOKEN)
        {
            module_configuration.module_name = input.substr(1);
            module_configuration.type = ModuleType::Conjuctor;
        }
        else
        {
            module_configuration.module_name = input;
            module_configuration.type = ModuleType::None;
        }

        for (std::string output; std::getline(line_stream, output, ',');)
        {

            output.erase(output.begin(), std::find_if(output.begin(), output.end(), [](int ch)
                                                      { return !std::isspace(ch); }));
            module_configuration.outputs.push_back(output);
        }
        if (module_configuration.type == ModuleType::None)
            configuration[input] = module_configuration;
        else
            configuration[input.substr(1)] = module_configuration;
    }

    return configuration;
}

struct pulse_t
{
    std::string from;
    bool value;
    std::string to;
};

std::string part1(std::stringstream &file_content)
{

    auto configurations = parseConfigurations(file_content);

    for (auto &pair : configurations)
    {
        auto &module_configuration = pair.second;
        for (auto &output : module_configuration.outputs)
            if (configurations[output].type == ModuleType::Conjuctor)
                configurations[output].input_pulse_values[module_configuration.module_name] = false;
    }

    std::queue<pulse_t> pulses;
    int high_pulses_count = 0;
    int low_pulses_count = 0;

    for (int i = 1000; i--;)
    {
        pulses.push({"button", false, "broadcaster"});
        while (!pulses.empty())
        {

            auto pulse = pulses.front();
            pulses.pop();
            if (pulse.value)
                high_pulses_count++;
            else
                low_pulses_count++;
            auto &module_configuration = configurations[pulse.to];
            if (module_configuration.type == ModuleType::FlipFlop)
            {
                if (pulse.value == false)
                {
                    module_configuration.state = !module_configuration.state;
                    for (auto &output : module_configuration.outputs)
                        pulses.push({pulse.to, module_configuration.state, output});
                }
            }
            else if (module_configuration.type == ModuleType::Conjuctor)
            {
                module_configuration.input_pulse_values[pulse.from] = pulse.value;
                bool value = std::accumulate(module_configuration.input_pulse_values.begin(), module_configuration.input_pulse_values.end(), true, [](bool acc, std::pair<std::string, bool> pair)
                                             { return acc && pair.second; });
                for (auto &output : module_configuration.outputs)
                    pulses.push({pulse.to, !value, output});
            }
            else
            {
                for (auto &output : module_configuration.outputs)
                    pulses.push({pulse.to, pulse.value, output});
            }
        }
    }

    return std::to_string(high_pulses_count * low_pulses_count);
}

std::string part2(std::stringstream &file_content)
{

    auto configurations = parseConfigurations(file_content);
    for (auto &pair : configurations)
    {
        auto &module_configuration = pair.second;
        for (auto &output : module_configuration.outputs)
            if (configurations[output].type == ModuleType::Conjuctor)
                configurations[output].input_pulse_values[module_configuration.module_name] = false;
    }

    std::string rx_conjuctor;
    for (auto &pair : configurations)
    {
        auto &module_configuration = pair.second;
        if (module_configuration.type == ModuleType::Conjuctor)
        {
            for (auto &output : module_configuration.outputs)
                if (output == "rx")
                {
                    rx_conjuctor = module_configuration.module_name;
                    break;
                }
        }
        if (!rx_conjuctor.empty())
            break;
    }

    std::queue<pulse_t> pulses;
    std::unordered_map<std::string, std::vector<int>> rx_conjuctor_high_pulse_indexes;

    for (int i = 0;; ++i)
    {

        std::unordered_map<std::string, bool> has_high_pulse;
        pulses.push({"button", false, "broadcaster"});
        while (!pulses.empty())
        {

            auto pulse = pulses.front();
            pulses.pop();

            if (pulse.to == rx_conjuctor && pulse.value == true)
                has_high_pulse[pulse.from] = true;

            auto &module_configuration = configurations[pulse.to];
            if (module_configuration.type == ModuleType::FlipFlop)
            {
                if (pulse.value == false)
                {
                    module_configuration.state = !module_configuration.state;
                    for (auto &output : module_configuration.outputs)
                        pulses.push({pulse.to, module_configuration.state, output});
                }
            }
            else if (module_configuration.type == ModuleType::Conjuctor)
            {
                module_configuration.input_pulse_values[pulse.from] = pulse.value;
                bool value = std::accumulate(module_configuration.input_pulse_values.begin(), module_configuration.input_pulse_values.end(), true, [](bool acc, std::pair<std::string, bool> pair)
                                             { return acc && pair.second; });
                for (auto &output : module_configuration.outputs)
                    pulses.push({pulse.to, !value, output});
            }
            else
            {
                for (auto &output : module_configuration.outputs)
                    pulses.push({pulse.to, pulse.value, output});
            }
        }

        for (auto ins : configurations[rx_conjuctor].input_pulse_values)
        {
            if (has_high_pulse[ins.first] != 0)
            {
                rx_conjuctor_high_pulse_indexes[ins.first].push_back(i);
            }
        };

        bool stop = std::all_of(configurations[rx_conjuctor].input_pulse_values.begin(), configurations[rx_conjuctor].input_pulse_values.end(), [&rx_conjuctor_high_pulse_indexes](std::pair<std::string, bool> pair)
                                { return rx_conjuctor_high_pulse_indexes[pair.first].size() >= 2; });
        if (stop)
            break;
    }

    unsigned long long int lcm = 1;
    for (auto ins : configurations[rx_conjuctor].input_pulse_values)
    {
        lcm = std::lcm(lcm, (unsigned long long int)(rx_conjuctor_high_pulse_indexes[ins.first][1] - rx_conjuctor_high_pulse_indexes[ins.first][0]));
    };

    return std::to_string(lcm);
}
