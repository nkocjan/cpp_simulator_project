#ifndef FIFA26_SIMULATOR_TEAM_H
#define FIFA26_SIMULATOR_TEAM_H

#include <string>
#include <vector>
#include <memory>
#include "Player.h"
#include "Statistic.h"

class Team {
private:
    std::string name;
    Statistic<int> points;
    Statistic<int> budget;

    // Team statistics
    Statistic<int> goalsScored;
    Statistic<int> goalsConceded;
    Statistic<int> matchesPlayed;
    Statistic<int> wins;
    Statistic<int> draws;
    Statistic<int> losses;

    std::vector<std::shared_ptr<Player>> players;

public:
    explicit Team(std::string _name, int _budget = 0);

    void addPlayer(std::shared_ptr<Player> player);

    double calculateTeamStrength() const;

    std::string getName() const;

    // Getters for statistics
    int getPoints() const;
    int getBudget() const;
    int getGoalsScored() const;
    int getGoalsConceded() const;
    int getMatchesPlayed() const;
    int getWins() const;
    int getDraws() const;
    int getLosses() const;

    // Setters for statistics
    void addPoints(int p);
    void setBudget(int b);
    void addGoalScored(int g = 1);
    void addGoalConceded(int g = 1);
    void addMatch();
    void addWin();
    void addDraw();
    void addLoss();

    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
};

#endif //FIFA26_SIMULATOR_TEAM_H