#pragma once
#include "Team.h"
#include "Statistic.h"
#include <memory>
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

    std::vector<std::string> matchEvents;

    void simulateCards();
    void simulateInjuries();
    void assignGoalsToPlayers(const std::shared_ptr<Team>& team, int goalCount);
    void assignCardsToPlayers(const std::shared_ptr<Team>& team, int cardCount, bool yellowCard);

public:
    /**
     * @brief Tworzy mecz.
     * @param home Druzyna gospodarzy.
     * @param away Druzyna gosci.
     */
    Match(std::shared_ptr<Team> home, std::shared_ptr<Team> away);

    /** @brief Symuluje przebieg i wynik meczu. */
    void simulate();

    /** @return Wskaznik na druzyne gospodarzy. */
    [[nodiscard]] std::shared_ptr<Team> getHomeTeam() const;
    /** @return Wskaznik na druzyne gosci. */
    [[nodiscard]] std::shared_ptr<Team> getAwayTeam() const;
    /** @return Liczba bramek gospodarzy. */
    [[nodiscard]] int getHomeGoals() const;
    /** @return Liczba bramek gosci. */
    [[nodiscard]] int getAwayGoals() const;
    /** @return true, gdy mecz zostal rozegrany. */
    [[nodiscard]] bool getIsFinished() const;
    /** @return Lista zdarzen meczowych w kolejnosci czasowej. */
    [[nodiscard]] const std::vector<std::string>& getMatchEvents() const;
};
