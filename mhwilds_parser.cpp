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
json filter_decorations(const json& input, const std::vector<std::string>& keywords) {
    json result = json::array();
    for (const auto& item : input) {
        if (!item.contains("skills")) continue;

        for (const auto& skill : item["skills"]) {
            std::string skill_name = skill["name"];
            for (const auto& keyword : keywords) {
                if (skill_name.find(keyword) != std::string::npos) {
                    result.push_back(item);
                    break;
                }
            }
        }
    }
    return result;
}

// Filter high rank armors
json filter_high_rank_armors(const json& input) {
    json result = json::array();
    for (const auto& item : input) {
        if (item.contains("rank") && item["rank"] == "high") {
            result.push_back(item);
        }
    }
    return result;
}

int main() {
    // === Decorations ===
    json decorations = load_json("decorations.json");
    auto filtered_decorations = filter_decorations(decorations, {"affinity", "attack", "stamina"});
    save_json("filtered_decorations.json", filtered_decorations);

    // === Armors ===
    json armors = load_json("armor.json");
    auto filtered_armors = filter_high_rank_armors(armors);
    save_json("filtered_armor.json", filtered_armors);

    std::cout << "Filtering complete. Check filtered_*.json files.\n";
    return 0;
}
