#ifndef FIFA26_SIMULATOR_LEAGUEENGINE_H
#define FIFA26_SIMULATOR_LEAGUEENGINE_H

#include <vector>
#include <memory>
#include <string>
#include "Team.h"
#include "Match.h"

/**
 * @file LeagueEngine.h
 * @brief Glowny silnik zarzadzajacy rozgrywkami ligowymi.
 */

/**
 * @brief Klasa menedzera ligi (druzyny, terminarz, symulacja, tabela).
 */
class LeagueEngine {
private:
    std::vector<std::shared_ptr<Team>> teams;
    std::shared_ptr<Team> playerTeam;
    std::vector<std::vector<std::shared_ptr<Match>>> schedule;
    int currentMatchday;

public:
    /** @brief Tworzy pusty silnik ligi. */
    LeagueEngine();

    /** @brief Dodaje druzyne do ligi. */
    void addTeam(std::shared_ptr<Team> team);
    /** @brief Ustawia druzyne gracza po nazwie. */
    void setPlayerTeam(const std::string& teamName);
    /**
     * @brief Generuje terminarz round-robin (mecz i rewanz) dla parzystej liczby druzyn.
     *
     * Gwarantuje, ze kazda para druzyn spotyka sie dokladnie dwa razy
     * (u siebie i na wyjezdzie), a kazda druzyna rozgrywa 2*(n-1) meczow.
     */
    void generateSchedule();

    /** @brief Symuluje nastepna kolejke. */
    void simulateNextMatchday();
    /** @return true, gdy wszystkie kolejki sa rozegrane. */
    [[nodiscard]] bool isLeagueFinished() const;

    /** @brief Sortuje tabele ligowa wedlug punktow i bilansu. */
    void sortTable();
    /** @brief Wypisuje tabele do konsoli. */
    void displayTable() const;
    /** @brief Eksportuje tabele i historie meczow do pliku. */
    void exportToFile(const std::string& filename) const;

    /** @return Lista druzyn ligi. */
    [[nodiscard]] const std::vector<std::shared_ptr<Team>>& getTeams() const;
    /** @return Wskaznik na druzyne gracza. */
    [[nodiscard]] std::shared_ptr<Team> getPlayerTeam() const;
    /** @return Terminarz w podziale na kolejki. */
    [[nodiscard]] const std::vector<std::vector<std::shared_ptr<Match>>>& getSchedule() const;
    /** @return Numer aktualnej kolejki (1-based logicznie, wewnetrznie liczony od 0). */
    [[nodiscard]] int getCurrentMatchday() const;
    /** @return Liczba wszystkich kolejek. */
    [[nodiscard]] int getTotalMatchdays() const;
};

#endif //FIFA26_SIMULATOR_LEAGUEENGINE_H
