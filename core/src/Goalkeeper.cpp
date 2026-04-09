#include "../headers/Goalkeeper.h"
#include <utility>

Goalkeeper::Goalkeeper(std::string _name, std::string _surname, double _condition, int _overall, int _defendingSkills)
    : Player(std::move(_name), std::move(_surname), _condition, _overall),
      defendingSkills("Defending Skills", _defendingSkills, 0, 99) {}

double Goalkeeper::calculateGoalChance() const {
    return (overall.getValue() * 0.7) + (defendingSkills.getValue() * 0.3) * (condition.getValue() / 100.0);
}

PlayerPosition Goalkeeper::getNaturalPosition() const {
    return PlayerPosition::Goalkeeper;
}

int Goalkeeper::getDefendingSkills() const {
    return defendingSkills.getValue();
}

void Goalkeeper::setDefendingSkills(int value) {
    defendingSkills.setValue(value);
}
