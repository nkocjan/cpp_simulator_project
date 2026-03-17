#include "../headers/Team.h"
#include <utility>

Team::Team(std::string _name, int _budget)
    : name(std::move(_name)),
      points("Points", 0, 0, 999999),
      budget("Budget", _budget, 0, 999999999),
      goalsScored("Goals Scored", 0, 0, 999),
      goalsConceded("Goals Conceded", 0, 0, 999),
      matchesPlayed("Matches Played", 0, 0, 9999),
      wins("Wins", 0, 0, 9999),
      draws("Draws", 0, 0, 9999),
      losses("Losses", 0, 0, 9999) {}

void Team::addPlayer(std::shared_ptr<Player> player) {
    if (player) {
        players.push_back(std::move(player));
    }
}

double Team::calculateTeamStrength() const {
    if (players.empty()) {
        return 0.0;
    }

    double totalStrength = 0.0;
    for (const auto& player : players) {
        totalStrength += player->calculateGoalChance();
    }
    
    return totalStrength / static_cast<double>(players.size());
}

std::string Team::getName() const {
    return name;
}

int Team::getPoints() const {
    return points.getValue();
}

int Team::getBudget() const {
    return budget.getValue();
}

int Team::getGoalsScored() const {
    return goalsScored.getValue();
}

int Team::getGoalsConceded() const {
    return goalsConceded.getValue();
}

int Team::getMatchesPlayed() const {
    return matchesPlayed.getValue();
}

int Team::getWins() const {
    return wins.getValue();
}

int Team::getDraws() const {
    return draws.getValue();
}

int Team::getLosses() const {
    return losses.getValue();
}

void Team::addPoints(int p) {
    points.add(p);
}

void Team::setBudget(int b) {
    budget.setValue(b);
}

void Team::addGoalScored(int g) {
    goalsScored.add(g);
}

void Team::addGoalConceded(int g) {
    goalsConceded.add(g);
}

void Team::addMatch() {
    matchesPlayed.add(1);
}

void Team::addWin() {
    wins.add(1);
}

void Team::addDraw() {
    draws.add(1);
}

void Team::addLoss() {
    losses.add(1);
}

const std::vector<std::shared_ptr<Player>>& Team::getPlayers() const {
    return players;
}