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
    int randomnessPercent;

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

    /** @brief Ustawia poziom losowosci symulacji (0-100). */
    void setRandomnessPercent(int value);
    /** @return Aktualny poziom losowosci symulacji. */
    [[nodiscard]] int getRandomnessPercent() const;

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

    /**
     * @brief Aktualizuje slot weekendowy pojedynczego meczu.
     * @param matchdayIndex Indeks kolejki (0-based).
     * @param matchIndex Indeks meczu w kolejce (0-based).
     * @param newSlot Nowy opis terminu.
     * @return true, jesli slot zostal zmieniony.
     */
    bool updateMatchSlot(size_t matchdayIndex, size_t matchIndex, const std::string& newSlot);

    /**
     * @brief Przeprowadza transfer zawodnika miedzy klubami.
     * @return true, jesli transfer zakonczyl sie sukcesem.
     */
    bool transferPlayer(const std::string& fromTeamName,
                        const std::string& toTeamName,
                        const std::string& playerName,
                        const std::string& playerSurname);
};

#endif
