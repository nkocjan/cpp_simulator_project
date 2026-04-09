#ifndef FIFA26_SIMULATOR_TEAM_H
#define FIFA26_SIMULATOR_TEAM_H

#include <array>
#include <memory>
#include <string>
#include <vector>

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

    Statistic<int> goalsScored;
    Statistic<int> goalsConceded;
    Statistic<int> matchesPlayed;
    Statistic<int> wins;
    Statistic<int> draws;
    Statistic<int> losses;

    std::vector<std::shared_ptr<Player>> players;
    std::array<std::shared_ptr<Player>, 4> startingLineup;

public:
    /**
     * @brief Tworzy druzyne.
     * @param _name Nazwa druzyny.
     */
    explicit Team(std::string _name);

    /** @brief Dodaje pilkarza do kadry. */
    void addPlayer(std::shared_ptr<Player> player);

    /** @return Srednia sila wszystkich zawodnikow w kadrze. */
    double calculateTeamStrength() const;
    /** @return Srednia sila przekazanej czworki meczowej.
     *  Uwzglednia oslabienie za gre poza naturalna pozycja i kondycje.
     */
    double calculateLineupStrength(const std::vector<std::shared_ptr<Player>>& lineup) const;

    /** @return Nazwa druzyny. */
    std::string getName() const;

    /** @return Liczba punktow druzyny. */
    int getPoints() const;
    /** @return Liczba strzelonych goli. */
    int getGoalsScored() const;
    /** @return Liczba straconych goli. */
    int getGoalsConceded() const;
    /** @return Liczba rozegranych meczow. */
    int getMatchesPlayed() const;
    /** @return Liczba zwyciestw. */
    int getWins() const;
    /** @return Liczba remisow. */
    int getDraws() const;
    /** @return Liczba porazek. */
    int getLosses() const;

    /** @brief Dodaje punkty. */
    void addPoints(int p);
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

    /** @return true, gdy zawodnik jest w kadrze druzyny. */
    bool hasPlayer(const std::shared_ptr<Player>& player) const;
    /** @brief Usuwa zawodnika z kadry i składu wyjsciowego. */
    bool removePlayer(const std::shared_ptr<Player>& player);

    /** @brief Ustawia domyslny sklad 4-osobowy wg pozycji i overall. */
    void setDefaultLineup();
    /** @brief Ustawia wskazanego gracza jako startera na podanej pozycji. */
    bool setStarter(PlayerPosition position, const std::shared_ptr<Player>& player);
    /** @return Starter z danej pozycji lub nullptr. */
    std::shared_ptr<Player> getStarter(PlayerPosition position) const;
    /** @return Lista rezerwowych (poza skladem wyjsciowym). */
    std::vector<std::shared_ptr<Player>> getBenchPlayers() const;

    /**
     * @brief Buduje sklad meczowy 4 zawodnikow.
     *
     * Jesli starter jest niedostepny, probuje zastapic go graczem z tej samej
     * pozycji, a gdy to niemozliwe - najlepszym dostepnym overall.
     * @param walkover Ustawiane na true, gdy nie da sie zestawic 4 graczy.
     * @return Lista zawodnikow wystawionych na mecz.
     */
    std::vector<std::shared_ptr<Player>> buildMatchLineup(bool& walkover) const;

    /** @brief Regeneruje kondycje pilkarzy, ktorzy nie zagrali w meczu. */
    void recoverConditionForNonPlaying(const std::vector<std::shared_ptr<Player>>& playedPlayers,
                                       double amount = 20.0);

    /** @brief Zmniejsza pauzy (kontuzje/zawieszenia) po rozegranej kolejce. */
    void decrementUnavailablePlayers();
};

#endif
