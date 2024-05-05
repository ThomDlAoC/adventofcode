#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <optional>

#define PART1_FILENAME "part1.txt"



std::vector<std::string> num_s = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
std::vector<int> num_d = {1,2,3,4,5,6,7,8,9};


std::optional<std::stringstream> readFileToStringStream(std::string path){
    std::ifstream file(path);
    if(!file.is_open()){
        std::cout << "Could not open file: " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return std::move(buffer);
}

int main(int argc, char const *argv[])
{

   
   
   //open file for reading
    std::optional<std::stringstream> buffer_opt = readFileToStringStream(PART1_FILENAME);

    if(buffer_opt == std::nullopt){
        return 1;
    }

    std::stringstream &buffer = *buffer_opt;
   
    {
    std::string line;
    int sum=0;
    while(std::getline(buffer, line))
    {
       int dfirst=0;
       int dlast=0;
       //get first and last digit in line
       for(char c : line)
       {
        if(c<'0' || c>'9')continue;
           if(dfirst==0)
           {
               dfirst=c-'0';
           }
           dlast=c-'0';
       }       
       sum += dfirst*10+dlast;
    }

    std::cout << "Sum of first and last digits: " << sum << std::endl;

}
    //part 2
     {
      
        buffer.clear();
        buffer.seekg(0, std::ios::beg);
    std::string line;
    int sum=0;
    while(std::getline(buffer, line))
    {

        std::string output_text;
        auto callback = [&](std::string const &m)
        {
            std::istringstream iss(m);
            // find index of m in num_s
            int index = -1;
            for (int i = 0; i < num_s.size(); i++)
            {
                if (num_s[i] == m)
                {
                    index = i;
                    break;
                }
            }
            if (index == -1)
                output_text += m;
            else
                output_text += std::to_string(num_d[index]);
        };

        std::regex re("one|two|three|four|five|six|seven|eight|nine");
        std::sregex_token_iterator
            begin(line.begin(), line.end(), re, {-1, 0}),
            end;
        std::for_each(begin, end, callback);


        int dfirst = 0;
        int dlast = 0;
        // get first and last digit in line
        for (char c : output_text)
        {
            if (c < '0' || c > '9')
                continue;
            if (dfirst == 0)
            {
                dfirst = c - '0';
            }
            dlast = c - '0';
       }       
       sum += dfirst*10+dlast;
    }

    std::cout << "Sum of first and last digits: " << sum << std::endl;

}
    



    return 0;
}
