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
#include <regex>

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

struct Aunt
{
    int children=-1;
    int cats=-1;
    int samoyeds=-1;
    int pomeranians=-1;
    int akitas=-1;
    int vizslas=-1;
    int goldfish=-1;
    int trees=-1;
    int cars=-1;
    int perfumes=-1;

};

std::vector<Aunt> parseAunts(std::stringstream &file_content)
{
    std::vector<Aunt> aunts;
    std::string line;
    int current_aunt = 0;
    while (std::getline(file_content, line))
    {
        current_aunt++;
        Aunt aunt;
        std::regex regex("([a-z]+): (\\d+)");
        std::smatch match;
        std::string::const_iterator searchStart(line.cbegin());
        while (std::regex_search(searchStart, line.cend(), match, regex))
        {
            std::string key = match[1];
            int value = std::stoi(match[2]);
            if (key == "children")
            {
                aunt.children = value;
            }
            else if (key == "cats")
            {
                aunt.cats = value;
            }
            else if (key == "samoyeds")
            {
                aunt.samoyeds = value;
            }
            else if (key == "pomeranians")
            {
                aunt.pomeranians = value;
            }
            else if (key == "akitas")
            {
                aunt.akitas = value;
            }
            else if (key == "vizslas")
            {
                aunt.vizslas = value;
            }
            else if (key == "goldfish")
            {
                aunt.goldfish = value;
            }
            else if (key == "trees")
            {
                aunt.trees = value;
            }
            else if (key == "cars")
            {
                aunt.cars = value;
            }
            else if (key == "perfumes")
            {
                aunt.perfumes = value;
            }
            searchStart = match.suffix().first;
        }
        aunts.push_back(aunt);
    }
    return aunts;
}

std::string part1(std::stringstream &file_content)
{
    std::vector<Aunt> aunts = parseAunts(file_content);

    Aunt reference_aunt={
        .children=3,
        .cats=7,
        .samoyeds=2,
        .pomeranians=3,
        .akitas=0,
        .vizslas=0,
        .goldfish=5,
        .trees=3,
        .cars=2,
        .perfumes=1
    };
    
    for (size_t i = 0; i < aunts.size(); i++)
    {
        Aunt aunt = aunts[i];
        if(aunt.children!=-1 &&  aunt.children != reference_aunt.children) continue;
        if(aunt.cats!=-1 &&  aunt.cats != reference_aunt.cats) continue;
        if(aunt.samoyeds!=-1 &&  aunt.samoyeds != reference_aunt.samoyeds) continue;
        if(aunt.pomeranians!=-1 &&  aunt.pomeranians != reference_aunt.pomeranians) continue;
        if(aunt.akitas!=-1 &&  aunt.akitas != reference_aunt.akitas) continue;
        if(aunt.vizslas!=-1 &&  aunt.vizslas != reference_aunt.vizslas) continue;
        if(aunt.goldfish!=-1 &&  aunt.goldfish != reference_aunt.goldfish) continue;
        if(aunt.trees!=-1 &&  aunt.trees != reference_aunt.trees) continue;
        if(aunt.cars!=-1 &&  aunt.cars != reference_aunt.cars) continue;
        if(aunt.perfumes!=-1 &&  aunt.perfumes != reference_aunt.perfumes) continue;

        
        return std::to_string(i+1);
    }
    return std::to_string(-1);
}

std::string part2(std::stringstream &file_content)
{
   std::vector<Aunt> aunts = parseAunts(file_content);

    Aunt reference_aunt={
        .children=3,
        .cats=7,
        .samoyeds=2,
        .pomeranians=3,
        .akitas=0,
        .vizslas=0,
        .goldfish=5,
        .trees=3,
        .cars=2,
        .perfumes=1
    };
    
    for (size_t i = 0; i < aunts.size(); i++)
    {
        const Aunt &aunt = aunts[i];
        if(aunt.children!=-1 &&  aunt.children != reference_aunt.children) continue;
        if(aunt.cats!=-1 &&  aunt.cats <= reference_aunt.cats) continue;
        if(aunt.samoyeds!=-1 &&  aunt.samoyeds != reference_aunt.samoyeds) continue;
        if(aunt.pomeranians!=-1 &&  aunt.pomeranians >= reference_aunt.pomeranians) continue;
        if(aunt.akitas!=-1 &&  aunt.akitas != reference_aunt.akitas) continue;
        if(aunt.vizslas!=-1 &&  aunt.vizslas != reference_aunt.vizslas) continue;
        if(aunt.goldfish!=-1 &&  aunt.goldfish >= reference_aunt.goldfish) continue;
        if(aunt.trees!=-1 &&  aunt.trees <= reference_aunt.trees) continue;
        if(aunt.cars!=-1 &&  aunt.cars != reference_aunt.cars) continue;
        if(aunt.perfumes!=-1 &&  aunt.perfumes != reference_aunt.perfumes) continue;

        
        return std::to_string(i+1);
    }
    return std::to_string(-1);
  
}
