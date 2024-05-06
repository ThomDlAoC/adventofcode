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

using distance_to_t = std::pair<std::string,int>;

std::tuple<std::string,std::string,int> stringToDistance(std::string &str){
    std::stringstream ss(str);
    std::string nop,from,to;
    int distance=0;
    ss>>from;
    ss>>nop;
    ss>>to;
    ss>>nop;
    ss>>nop;
    distance = std::stoi(nop);
    return std::make_tuple(from,to,distance);
    
}

int bfs(std::unordered_map<std::string,std::vector<distance_to_t>> &mappy,const std::string &from,const std::unordered_set<std::string> &cities,std::unordered_set<std::string> &visited,int defaultValue,std::function<int(int,int)> func)
{
    if(visited.size()==cities.size())
        return 0;

    visited.insert(from);
    int resDist=defaultValue;
    for(const auto &[city,distance] : mappy[from])    
        if(!visited.contains(city))        
            resDist=func(resDist,distance+bfs(mappy,city,cities,visited,defaultValue,func));        
    
    visited.erase(from);
    return resDist;
}

std::string part1(std::stringstream &file_content)
{

    std::unordered_map<std::string,std::vector<distance_to_t>> mappy;
    std::unordered_set<std::string> cities;
    for(std::string line; std::getline(file_content,line);)
    {   
        const auto [from,to,distance] = stringToDistance(line);
        mappy[from].push_back({to,distance});
        mappy[to].push_back({from,distance});
        cities.insert(from);
    }

    int minDist=INT_MAX;
    for(const auto &from : cities)
    {
        std::unordered_set<std::string> visited;
        minDist=std::min(minDist,bfs(mappy,from,cities,visited,INT_MAX,[](int a,int b){return std::min(a,b);}));
    }

    return std::to_string(minDist);
}

std::string part2(std::stringstream &file_content)
{

    
    std::unordered_map<std::string,std::vector<distance_to_t>> mappy;
    std::unordered_set<std::string> cities;
    for(std::string line; std::getline(file_content,line);)
    {   
        const auto [from,to,distance] = stringToDistance(line);
        mappy[from].push_back({to,distance});
        mappy[to].push_back({from,distance});
        cities.insert(from);
    }

    int maxDist=0;
    for(const auto &from : cities)
    {
        std::unordered_set<std::string> visited;
        maxDist=std::max(maxDist,bfs(mappy,from,cities,visited,0,[](int a,int b){return std::max(a,b);}));
    }

    return std::to_string(maxDist);
}
