#pragma once
#include "Team.h"
#include "Statistic.h"
#include <memory>
#include <string>
#include <vector>

class Match {
private:
    std::shared_ptr<Team> homeTeam;
    std::shared_ptr<Team> awayTeam;
    Statistic<int> homeGoals;
    Statistic<int> awayGoals;
    bool isFinished;

    std::vector<std::string> matchEvents;

    // Helper methods for simulating events
    void simulateCards();
    void simulateInjuries();
    void assignGoalsToPlayers(const std::shared_ptr<Team>& team, int goalCount);
    void assignCardsToPlayers(const std::shared_ptr<Team>& team, int cardCount, bool yellowCard);

public:
    Match(std::shared_ptr<Team> home, std::shared_ptr<Team> away);

    void simulate();

    [[nodiscard]] std::shared_ptr<Team> getHomeTeam() const;
    [[nodiscard]] std::shared_ptr<Team> getAwayTeam() const;
    [[nodiscard]] int getHomeGoals() const;
    [[nodiscard]] int getAwayGoals() const;
    [[nodiscard]] bool getIsFinished() const;
    [[nodiscard]] const std::vector<std::string>& getMatchEvents() const;
};
