#include "../headers/Player.h"
#include <utility>

Player::Player(std::string _name, std::string _surname, double _condition, int _overall)
    : name(std::move(_name)), surname(std::move(_surname)),
      condition("Condition", _condition, 0.0, 100.0),
      overall("Overall", _overall, 0, 99),
      goals("Goals", 0, 0, 1000),
      assists("Assists", 0, 0, 1000),
      yellowCards("Yellow Cards", 0, 0, 5),
      redCards("Red Cards", 0, 0, 2),
      minutesPlayed("Minutes Played", 0, 0, 999999),
      morale("Morale", 75.0, 0.0, 100.0),
      injuryDuration(0) {}

std::string Player::getName() const {
    return name;
}

std::string Player::getSurname() const {
    return surname;
}

double Player::getCondition() const {
    return condition.getValue();
}

int Player::getOverall() const {
    return overall.getValue();
}

int Player::getGoals() const {
    return goals.getValue();
}

int Player::getAssists() const {
    return assists.getValue();
}

int Player::getYellowCards() const {
    return yellowCards.getValue();
}

int Player::getRedCards() const {
    return redCards.getValue();
}

int Player::getMinutesPlayed() const {
    return minutesPlayed.getValue();
}

double Player::getMorale() const {
    return morale.getValue();
}

int Player::getInjuryDuration() const {
    return injuryDuration;
}

void Player::setCondition(double value) {
    condition.setValue(value);
}

void Player::setOverall(int value) {
    overall.setValue(value);
}

void Player::addGoal() {
    goals.add(1);
}

void Player::addAssist() {
    assists.add(1);
}

void Player::addYellowCard() {
    yellowCards.add(1);
}

void Player::addRedCard() {
    redCards.add(1);
}

void Player::addMinutesPlayed(int minutes) {
    minutesPlayed.add(minutes);
}

void Player::setMorale(double value) {
    morale.setValue(value);
}

void Player::setInjuryDuration(int duration) {
    injuryDuration = duration;
}
