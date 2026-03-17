#ifndef FIFA26_SIMULATOR_LEAGUEENGINE_H
#define FIFA26_SIMULATOR_LEAGUEENGINE_H

#include <vector>
#include <memory>
#include <string>
#include "Team.h"
#include "Match.h"

class LeagueEngine {
private:
    std::vector<std::shared_ptr<Team>> teams;
    std::shared_ptr<Team> playerTeam;
    std::vector<std::vector<std::shared_ptr<Match>>> schedule;
    int currentMatchday;

public:
    LeagueEngine();

    // Configuration methods
    void addTeam(std::shared_ptr<Team> team);
    void setPlayerTeam(const std::string& teamName);
    void generateSchedule();

    // Simulation methods
    void simulateNextMatchday();
    [[nodiscard]] bool isLeagueFinished() const;

    // Presentation and export methods
    void sortTable();
    void displayTable() const;
    void exportToFile(const std::string& filename) const;

    // Getters
    [[nodiscard]] const std::vector<std::shared_ptr<Team>>& getTeams() const;
    [[nodiscard]] std::shared_ptr<Team> getPlayerTeam() const;
    [[nodiscard]] const std::vector<std::vector<std::shared_ptr<Match>>>& getSchedule() const;
    [[nodiscard]] int getCurrentMatchday() const;
    [[nodiscard]] int getTotalMatchdays() const;
};

#endif //FIFA26_SIMULATOR_LEAGUEENGINE_H

