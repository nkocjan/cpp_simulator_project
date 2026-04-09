#pragma once
#include "Team.h"
#include "Statistic.h"
#include <memory>
#include <set>
#include <string>
#include <vector>

/**
 * @file Match.h
 * @brief Deklaracja klasy pojedynczego meczu.
 */

/**
 * @brief Reprezentuje mecz pomiedzy dwiema druzynami.
 */
class Match {
private:
    std::shared_ptr<Team> homeTeam;
    std::shared_ptr<Team> awayTeam;
    Statistic<int> homeGoals;
    Statistic<int> awayGoals;
    bool isFinished;
    int randomnessPercent;
    std::string weekendSlot;

    std::vector<std::string> matchEvents;
    std::vector<std::shared_ptr<Player>> homeMatchPlayers;
    std::vector<std::shared_ptr<Player>> awayMatchPlayers;
    std::set<const Player*> homeScorers;
    std::set<const Player*> awayScorers;

    void simulateCards();
    void simulateInjuries();
    void assignGoalsToPlayers(const std::vector<std::shared_ptr<Player>>& lineup, int goalCount, std::set<const Player*>& scorers);
    void assignCardsToPlayers(const std::vector<std::shared_ptr<Player>>& lineup, int cardCount, bool yellowCard);
    void applyTeamMorale(const std::shared_ptr<Team>& team,
                         const std::vector<std::shared_ptr<Player>>& playedPlayers,
                         const std::set<const Player*>& scorers,
                         bool won,
                         bool lost);
    void applyPostMatchMorale(int homeGoalCount, int awayGoalCount);

public:
    /**
     * @brief Tworzy mecz.
     * @param home Druzyna gospodarzy.
     * @param away Druzyna gosci.
     * @param randomness Udzial losowosci w skali 0-100.
     */
    Match(std::shared_ptr<Team> home, std::shared_ptr<Team> away, int randomness = 50);

    /** @brief Symuluje przebieg i wynik meczu. */
    void simulate();

    /** @return Zaplanowany slot weekendowy meczu. */
    [[nodiscard]] const std::string& getWeekendSlot() const;
    /**
     * @brief Ustawia slot weekendowy meczu.
     * @return true, jesli slot zostal zmieniony.
     */
    bool setWeekendSlot(const std::string& slot);

    /** @return Druzyna gospodarzy. */
    [[nodiscard]] std::shared_ptr<Team> getHomeTeam() const;
    /** @return Druzyna gosci. */
    [[nodiscard]] std::shared_ptr<Team> getAwayTeam() const;
    /** @return Liczba goli gospodarzy. */
    [[nodiscard]] int getHomeGoals() const;
    /** @return Liczba goli gosci. */
    [[nodiscard]] int getAwayGoals() const;
    /** @return true, gdy mecz jest zakonczony. */
    [[nodiscard]] bool getIsFinished() const;
    /** @return Lista zdarzen opisujacych przebieg meczu. */
    [[nodiscard]] const std::vector<std::string>& getMatchEvents() const;
};
