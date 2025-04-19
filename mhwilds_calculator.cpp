#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Desired skills to filter by
const std::vector<std::string> desired_skills = {"Evade Extender"};
const std::vector<int> desired_level = {3};

struct build{
    json skills;
    int damage;
    int number;
    EquipmentPiece armor [5];
    json decorations;
    build* fathernode;
    build* childnode;
    bool is_root;
};

struct EquipmentPiece {
    std::string name;
    std::vector<int> slots;  // e.g. [3, 1]
    json skills;

    
    void print()
    {
        std::cout << "Name: " << name;
        std::cout << " Slots: ";
        for (int i = 0; i < slots.size(); i++)
        {
            std::cout << slots[i] << " ";
        }
        std::cout << "skills: ";
        for (const auto& skill: skills)
        {
            std::cout << skill["skill"]["name"] << " lv: " << skill["level"] << " ";
        }
        std::cout << std::endl;
    }
};

// Load armor pieces by type
std::vector<EquipmentPiece> load_armor(const json& data, const std::string& type) {
    std::vector<EquipmentPiece> pieces;
    for (const auto& item : data) {
        if (item["type"] != type || item["rank"] != "high") continue;

        EquipmentPiece piece;
        piece.name = item["name"];
        for (const auto& slot : item["slots"]) {
            piece.slots.push_back(slot.get<int>());
        }
        piece.skills = item["skills"];
        pieces.push_back(piece);
    }
    return pieces;
}


void insert_node(build* new_build, build* old_build)
{
    while(new_build->damage>old_build->damage && !old_build->is_root)
    {
        old_build->number++;
        old_build = old_build->fathernode;
    }
    new_build->childnode = old_build;
    if(!old_build->is_root)//if old_build is not root
    {
        new_build->fathernode = old_build->fathernode;
        old_build->fathernode->childnode=new_build;
        new_build->number = old_build->number -1;
    }
    else
    {
        new_build->fathernode = nullptr;
        new_build->is_root = true;
        old_build->is_root = false;
        new_build->number=0;
    }
    old_build->fathernode = new_build;
}

// Placeholder for calculating attack score from skills
int compute_attack(const json& total_skills, int base_attack) {
    int bonus = 0;
    for (const auto& skill : total_skills) {
        std::string name = skill["skill"]["name"];
        int level = skill["level"];

        if (name == "Attack Boost") {
            bonus += 3 * level;
        }
        // Add more logic here...
    }
    return base_attack + bonus;
}

// Check if build includes all desired skills
bool has_required_skills(const json& total_skills) {
    for (const auto& required : desired_skills) {
        bool found = false;
        for (const auto& skill : total_skills) {
            if (skill["skill"]["name"] == required) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

int base_attack_calc(build& new_build)
{

}

int main() {
    json armors, decorations, weapon;

    // Load filtered JSON data
    std::ifstream armorFile("armor_filtered.json");
    std::ifstream decoFile("decorations_filtered.json");
    std::ifstream weaponFile("weapon.json");

    armorFile >> armors;
    decoFile >> decorations;
    weaponFile >> weapon;

    auto heads = load_armor(armors, "head");
    auto chests = load_armor(armors, "chest");
    auto arms = load_armor(armors, "arms");
    auto waists = load_armor(armors, "waist");
    auto legs = load_armor(armors, "legs");

    int best_attack = 0;
    json best_build;
    build best_builds[1000];
    json total_skills = json::array();
    std::vector<int> total_slots;


    // Brute-force combination (can optimize later)
    for (const auto& head : heads)
    for (const auto& chest : chests)
    for (const auto& arm : arms)
    for (const auto& waist : waists)
    for (const auto& leg : legs)
    {
        total_skills.clear();
        total_slots.clear();
        // Combine all skills and slots

        auto add_slots = [&](const EquipmentPiece& p) {
            total_slots.insert(total_slots.end(), p.slots.begin(), p.slots.end());
        };
        auto add_skills = [&](const json& skills) {
            for (const auto& s : skills) total_skills.push_back(s);
        };

        add_slots(head); add_skills(head.skills);
        add_slots(chest); add_skills(chest.skills);
        add_slots(arm); add_skills(arm.skills);
        add_slots(waist); add_skills(waist.skills);
        add_slots(leg); add_skills(leg.skills);

        if (!has_required_skills(total_skills)) continue;
        // Add weapon stats
        int base_attack = weapon["attack"]["raw"];
        if (weapon.contains("slots")) {
            for (const auto& s : weapon["slots"]) {
                total_slots.push_back(s.get<int>());
            }
        }

        // TODO: Add decoration fitting logic (fit into available slots)

        // Check if this build has the required skills

        int total_attack = compute_attack(total_skills, base_attack); //remember to special case critical boost

        if (total_attack > best_attack) {
            best_attack = total_attack;
            best_build = {
                {"head", head.name}, {"chest", chest.name},
                {"arms", arm.name}, {"waist", waist.name}, {"legs", leg.name},
                {"attack", total_attack}
            };
        }
    }

    std::cout << "Best build:\n" << best_build.dump(4) << std::endl;

    return 0;
}
