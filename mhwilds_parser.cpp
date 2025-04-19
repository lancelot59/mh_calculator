#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

// Utility to load a JSON file
json load_json(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return {};
    }

    json j;
    file >> j;
    return j;
}

// Save JSON to file
void save_json(const std::string& filename, const json& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data.dump(2);  // Pretty print with indent of 2
        file.close();
    }
}

// Filter decorations by keywords
json filter_decorations(const json& input, const std::vector<std::string>& keywords) 
{
    json result = json::array();
    bool skip = false;
    for (const auto& item : input) {

        for (const auto& skillEntry : item["skills"]) 
        {
            // Check the skill name and description
            std::string skillName = skillEntry["skill"]["name"].get<std::string>();
            std::string skillDesc = skillEntry["description"].get<std::string>();

            for (const auto& keyword : keywords) {
                // Case-insensitive check (optional: can make both lowercase)
                if (skillName.find(keyword) != std::string::npos ||
                    skillDesc.find(keyword) != std::string::npos) {
                    result.push_back(item);
                    skip = true;
                    break; 
                }
            }
            if(skip)
                skip = false;
                break;
        }
    }
    return result;
}

json filter_charms(const json& input, const std::vector<std::string>& keywords) 
{
    json result = json::array();
    bool skip = false;
    for (const auto& item : input) 
    {

        for (const auto& rank : item["ranks"]) 
        {

            for (const auto& skillEntry : rank["skills"]) 
            {
                std::string skillName = skillEntry["skill"]["name"].get<std::string>();
                std::string skillDesc = skillEntry["description"].get<std::string>();

                for (const auto& keyword : keywords)
                {
                    // Case-insensitive check (optional: can make both lowercase)
                    if (skillName.find(keyword) != std::string::npos || skillDesc.find(keyword) != std::string::npos) 
                        {
                        result.push_back(item);
                        skip = true;
                        break; 
                    }
                }
                if(skip)
                    break;
            }
            if(skip)
                skip = false;
                break;
        }
    }
    return result;
}


// Filter high rank armors
json filter_armors(const json& input) {
    json result = json::array();
    for (const auto& item : input) {
        if (item.contains("rank") && item["rank"] == "high") {
            result.push_back(item);
        }
    }
    return result;
}
json filter_weapons(const json& input, const std::vector<int> filters)
{
    json result = json::array();
    for(const auto& item: input)
    {
        for(const auto& filter: filters)
        {
            if(item["rarity"] == filter)
            result.push_back(item);
        }    
    }
    return result;
}

const std::vector<std::string> decoration_filters = {"affinity", "attack", "stamina", "Affinity", "Attack", "Stamina"};
const std::vector<std::string> charm_filters = {"affinity", "attack", "stamina", "Affinity", "Attack", "Stamina"};
const std::vector<int> weapon_filters = {7,8};
int main() {
    // === Decorations ===
    json decorations = load_json("decorations.json");
    auto filtered_decorations = filter_decorations(decorations, decoration_filters);
    save_json("filtered_decorations.json", filtered_decorations);

    // === Armors ===
    json armors = load_json("armor.json");
    auto filtered_armors = filter_armors(armors);
    save_json("filtered_armor.json", filtered_armors);

    // === charms ===
    json charms = load_json("charms.json");
    auto filtered_charms = filter_charms(charms,charm_filters);
    save_json("filtered_charms.json", filtered_charms);

    json weapons = load_json("weapons.json");
    auto filtered_weapons = filter_weapons(weapons, weapon_filters);
    save_json("filtered_weapons.json", filtered_weapons);

    std::cout << "Filtering complete. Check filtered_*.json files.\n";
    return 0;
}
