#include "../headers/Midfielder.h"
#include <utility>

Midfielder::Midfielder(std::string _name, std::string _surname, double _condition, int _overall, int _passingVision)
    : Player(std::move(_name), std::move(_surname), _condition, _overall),
      passingVision("Passing Vision", _passingVision, 0, 99) {}

double Midfielder::calculateGoalChance() const {
    return (overall.getValue() * 0.55) + (passingVision.getValue() * 0.45) * (condition.getValue() / 100.0);
}

PlayerPosition Midfielder::getNaturalPosition() const {
    return PlayerPosition::Midfielder;
}

int Midfielder::getPassingVision() const {
    return passingVision.getValue();
}

void Midfielder::setPassingVision(int value) {
    passingVision.setValue(value);
}
