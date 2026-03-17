#include "../headers/Striker.h"
#include <utility>

Striker::Striker(std::string _name, std::string _surname, double _condition, int _overall, int _shootingFinishing)
    : Player(std::move(_name), std::move(_surname), _condition, _overall),
      shootingFinishing("Shooting Finishing", _shootingFinishing, 0, 99) {}

double Striker::calculateGoalChance() const {
    return (overall.getValue() * 0.5) + (shootingFinishing.getValue() * 0.5) * (condition.getValue() / 100.0);
}

int Striker::getShootingFinishing() const {
    return shootingFinishing.getValue();
}

void Striker::setShootingFinishing(int value) {
    shootingFinishing.setValue(value);
}
