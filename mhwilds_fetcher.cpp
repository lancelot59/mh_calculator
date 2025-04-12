#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>

using json = nlohmann::json;

const int size = 7;
std::string type[size] = {"armor", "charms", "decorations", "skills", "weapons", "monsters", /*"armor/sets"*/} ;
//std::string type [1] = {"armor/sets"};
int main() {
    // Replace with actual API endpoint for weapons
    for(int i = 0; i < size; i++)
    {
        std::string url = "https://wilds.mhdb.io/en/"+type[i];

        // Send GET request
        cpr::Response response = cpr::Get(cpr::Url{url}, cpr::VerifySsl(false));
    
        // Check for successful response
        if (response.status_code != 200) {
            std::cerr << "Failed to fetch data. HTTP Status: " << response.status_code << std::endl;
            return 1;
        }
    
        std::ofstream outfile(type[i]+".json");
        if (!outfile.is_open())
        {
            std::ofstream errorlog("error.log");
            errorlog << "failed to fetch data. HTTP status: " << response.status_code << "\n";
            errorlog << "http request: " << url << "\n";
            errorlog << "Error message: " << response.error.message << "\n";
            return 1;
        }
        outfile << response.text;
        outfile.close();
    }

    return 0;
}
