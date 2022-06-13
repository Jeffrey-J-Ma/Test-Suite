#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

std::string exec_command(const char* cmd) 
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) 
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
    {
        result += buffer.data();
    }
    return result;
}

std::string extractData(const std::string str, std::string type) 
{
    std::stringstream output;
    bool found = false;
    std::vector<std::string> searches;
    std::vector<std::vector<std::string> > vec;
    std::vector<std::string> temp_vec;
    std::vector<std::string> data;
    std::stringstream iss(str);
    std::string line;
    int i = 0; 
    searches.push_back("TotalNode");
    searches.push_back("TotalEdge");
    searches.push_back("AvgIndOutDeg");
    searches.push_back("AvgIndInDeg");
    searches.push_back("AvgOutDegree");
    searches.push_back("AvgInDegree");
    searches.push_back("AvgWeight");
    searches.push_back("AvgTopLvlPtsSize");
    if(type == "read_write_svfg")
    {
        temp_vec.push_back("*********Andersen Pointer Analysis Stats***************"); 
        temp_vec.push_back(searches[7]);
        vec.push_back(temp_vec);
        temp_vec.clear();
        temp_vec.push_back("****Persistent Points-To Cache Statistics: bv-finalize****");
        vec.push_back(temp_vec);
        temp_vec.clear();
        temp_vec.push_back("****SVFG Statistics****"); 
        for (int x = 0; x < (searches.size() - 1); x++)
        {
            temp_vec.push_back(searches[x]); 
        }   
        vec.push_back(temp_vec);
    } 
    else 
    {   
       temp_vec.push_back("*********Andersen Pointer Analysis Stats***************"); 
       temp_vec.push_back(searches[7]);
       vec.push_back(temp_vec);
    }

        std::string marker = vec[0][0];
        // extract data from command output
        while(getline(iss, line)) 
        {
            if (line.find("#######################################################") != std::string::npos && found){
                 found = false;
                 i++;
                 marker = (i < vec.size()) ? vec[i][0] : "";
                 if(marker == "") break; 
            }
            if (data.size() >= searches.size())
                break;
            if (line.find(marker) != std::string::npos){
                found = true;
            }
            if (found) {
                for (int j = 1; j < vec[i].size(); j++)
                {  
                    if (line.find(vec[i][j]) != std::string::npos) 
                    {
                        data.push_back(line);
                    }
                }
            }
        }

    for(int i = 0; i < data.size(); i++){
        output << data[i] << "\n";
    }
    return output.str();
}

int main(int argc, char *argv[])
{

    std::string cmd1 = argv[1];
    std::string cmd2 = argv[2];
    std::string folder = argv[3];
    std::string result, result2;
    result = exec_command(("./" + cmd1 + " " +  folder).c_str());
    if (argc == 5)
    {
        std::string folder2 = argv[4];
        result2 = exec_command(("./" + cmd2 + " " +  folder2).c_str());
    }
    else 
    {
        result2 = exec_command(("./" + cmd2 + " " +  folder).c_str());
    }

    std::string data1;
    std::string data2; 

    std::size_t read_write_svfg = cmd1.find("write-svfg");
    if (read_write_svfg!=std::string::npos)
    {
        data1 = extractData(result, "read_write_svfg");
        data2 = extractData(result2, "read_write_svfg");
    } 
    else 
    { 
        data1 = extractData(result, "ander_nander,read_write_ander");
        data2 = extractData(result2, "ander_nander,read_write_ander");
    }

    std::cout << data1.compare(data2) << std::endl;
}
