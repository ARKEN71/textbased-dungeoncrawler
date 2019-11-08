#include "Player.hpp"

#include <iomanip> // std::setw
#include <fstream>
#include "nlohmann/json.hpp"

bool Player::Load(std::string filename, Player& player) {
    // read from file
    std::ifstream fileIn(filename);
    if (!fileIn) {
        return false;
    }

    nlohmann::json jIn;
    fileIn >> jIn;
    fileIn.close();
    if (!jIn.is_object()) {
        return false;
    }
    
    // deserialise into {player}
    player.m_baseHp = 10;
    player.m_strength = jIn["strength"];
    player.m_dexterity = jIn["dexterity"];
    player.m_constitution = jIn["constitution"];
    player.m_level = jIn["level"];
    player.m_gold = jIn["gold"];
    player.m_exp = jIn["experience"];
    player.m_currentHp = player.GetMaximumHp();
    player.m_weapon = AvailableWeapons.at(Weapon::GetFromName(jIn["weapon"]));
    player.m_itemInventory = ItemInventory::Deserialise(jIn["inventory"]);
    
    return true;
}

bool Player::Save(std::string filename) {
    // serialise to json
    nlohmann::json jOut;
    jOut["strength"] = m_strength;
    jOut["dexterity"] = m_dexterity;
    jOut["constitution"] = m_constitution;
    jOut["level"] = m_level;
    jOut["gold"] = m_gold;
    jOut["experience"] = m_exp;
    jOut["weapon"] = m_weapon.GetName();
    jOut["inventory"] = m_itemInventory.Serialise();

    // write to file
    std::ofstream fileOut(filename);
    if (!fileOut) {
        return false;
    }
    fileOut << std::setw(2) << jOut << std::endl;
    fileOut.close();

    return true;
}

Player::Player(std::string& name)
    : Creature(name, 10, AvailableWeapons.at(WEAPONS::UNARMED)) {
    m_strength = 0;
    m_dexterity = 0;
    m_constitution = 0;
}

Player::~Player() {
    
}

bool Player::Pay(unsigned int goldCost) {
    if (m_gold >= goldCost) {
        m_gold -= goldCost;
        return true;
    }
    return false;
}

unsigned int Player::LevelUpCost() const {
    // gets gradually more expensive
    return 10u + pow((double)m_level, 1.1);
}

bool Player::LevelUp(CREATURE_ABILITIES ability) {
    if (m_exp >= LevelUpCost()) {
        switch (ability) {
            case CREATURE_ABILITIES_STRENGTH: m_strength++; break;
            case CREATURE_ABILITIES_DEXTERITY: m_dexterity++; break;
            case CREATURE_ABILITIES_CONSTITUTION: m_constitution++; break;
            default: break;
        }
        m_exp -= LevelUpCost();
        m_level++;
        return true;
    }
    return false;
}

void Player::Reward(unsigned int gold, unsigned int exp) {
    m_gold += gold;
    m_exp += exp;
}

std::string Player::ToString() const {
    return Creature::ToString() + "\nLevel: " + std::to_string(m_level) + " | Gold: " + std::to_string(m_gold) + "g | Exp: " + std::to_string(m_exp);
}
