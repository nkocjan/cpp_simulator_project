#include "../headers/Defender.h"
#include <utility>

Defender::Defender(std::string _name, std::string _surname, double _condition, int _overall, int _defendingSkills)
    : Player(std::move(_name), std::move(_surname), _condition, _overall),
      defendingSkills("Defending Skills", _defendingSkills, 0, 99) {}

double Defender::calculateGoalChance() const {
    return (overall.getValue() * 0.6) + (defendingSkills.getValue() * 0.4) * (condition.getValue() / 100.0);
}

PlayerPosition Defender::getNaturalPosition() const {
    return PlayerPosition::Defender;
}

int Defender::getDefendingSkills() const {
    return defendingSkills.getValue();
}

void Defender::setDefendingSkills(int value) {
    defendingSkills.setValue(value);
}
