#ifndef FIFA26_SIMULATOR_TEAM_H
#define FIFA26_SIMULATOR_TEAM_H

#include <string>
#include <vector>
#include <memory>
#include "Player.h"
#include "Statistic.h"

/**
 * @file Team.h
 * @brief Deklaracja klasy druzyny.
 */

/**
 * @brief Reprezentuje druzyne ligowa i jej statystyki zbiorcze.
 */
class Team {
private:
    std::string name;
    Statistic<int> points;
    Statistic<int> budget;

    Statistic<int> goalsScored;
    Statistic<int> goalsConceded;
    Statistic<int> matchesPlayed;
    Statistic<int> wins;
    Statistic<int> draws;
    Statistic<int> losses;

    std::vector<std::shared_ptr<Player>> players;

public:
    /**
     * @brief Tworzy druzyne.
     * @param _name Nazwa druzyny.
     * @param _budget Budzet startowy.
     */
    explicit Team(std::string _name, int _budget = 0);

    /** @brief Dodaje pilkarza do kadry. */
    void addPlayer(std::shared_ptr<Player> player);

    /** @return Srednia sila druzyny wyliczona na podstawie pilkarzy. */
    double calculateTeamStrength() const;

    /** @return Nazwa druzyny. */
    std::string getName() const;

     int getPoints() const;
     int getBudget() const;
     int getGoalsScored() const;
     int getGoalsConceded() const;
     int getMatchesPlayed() const;
     int getWins() const;
     int getDraws() const;
     int getLosses() const;

    /** @brief Dodaje punkty. */
    void addPoints(int p);
    /** @brief Ustawia budzet. */
    void setBudget(int b);
    /** @brief Zwieksza liczbe strzelonych goli. */
    void addGoalScored(int g = 1);
    /** @brief Zwieksza liczbe straconych goli. */
    void addGoalConceded(int g = 1);
    /** @brief Zwieksza licznik meczow. */
    void addMatch();
    /** @brief Zwieksza licznik zwyciestw. */
    void addWin();
    /** @brief Zwieksza licznik remisow. */
    void addDraw();
    /** @brief Zwieksza licznik porazek. */
    void addLoss();

    /** @return Lista pilkarzy druzyny. */
    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
};

#endif