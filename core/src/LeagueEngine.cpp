#include "../headers/LeagueEngine.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

LeagueEngine::LeagueEngine() : currentMatchday(0) {}

void LeagueEngine::addTeam(std::shared_ptr<Team> team) {
    if (team) {
        teams.push_back(std::move(team));
    }
}

void LeagueEngine::setPlayerTeam(const std::string& teamName) {
    for (const auto& team : teams) {
        if (team->getName() == teamName) {
            playerTeam = team;
            return;
        }
    }
    // If not found, log warning
    std::cerr << "Drużyna " << teamName << " nie znaleziona w lidze!" << std::endl;
}

void LeagueEngine::generateSchedule() {
    // Check if we have even number of teams (required for round-robin)
    if (teams.empty()) {
        std::cerr << "Nie można wygenerować terminarz - brak drużyn w lidze!" << std::endl;
        return;
    }

    if (teams.size() % 2 != 0) {
        std::cerr << "Liczba drużyn musi być parzysta dla algorytmu round-robin!" << std::endl;
        return;
    }

    schedule.clear();

    size_t n = teams.size();

    // Generate home matches for first round
    for (int round = 0; round < 2; ++round) {
        for (size_t matchday = 0; matchday < n - 1; ++matchday) {
            std::vector<std::shared_ptr<Match>> matchdayMatches;

            for (size_t i = 0; i < n / 2; ++i) {
                size_t homeIdx = i;
                size_t awayIdx = n - 1 - i;

                // Rotate teams for next matchday (except last team which is fixed)
                if (matchday > 0) {
                    homeIdx = (homeIdx + matchday) % (n - 1);
                    if (homeIdx == 0) homeIdx = 0;
                    awayIdx = (awayIdx + matchday) % (n - 1);
                    if (awayIdx == 0) awayIdx = n - 1;
                    else awayIdx = (n - 1 - (awayIdx - 1));
                }

                // Swap home/away for second round
                if (round == 1) {
                    std::swap(homeIdx, awayIdx);
                }

                // Ensure valid indices
                if (homeIdx >= teams.size() || awayIdx >= teams.size() || homeIdx == awayIdx) {
                    continue;
                }

                auto match = std::make_shared<Match>(teams[homeIdx], teams[awayIdx]);
                matchdayMatches.push_back(std::move(match));
            }

            if (!matchdayMatches.empty()) {
                schedule.push_back(std::move(matchdayMatches));
            }
        }
    }

    std::cout << "Terminarz wygenerowany: " << schedule.size() << " kolejek" << std::endl;
}

void LeagueEngine::simulateNextMatchday() {
    if (isLeagueFinished()) {
        std::cerr << "Liga się już skończyła!" << std::endl;
        return;
    }

    if (currentMatchday >= schedule.size()) {
        std::cerr << "Błąd: currentMatchday przekroczyło rozmiar schedule!" << std::endl;
        return;
    }

    std::cout << "\n=== KOLEJKA " << (currentMatchday + 1) << " ===" << std::endl;

    // Simulate all matches in current matchday
    for (auto& match : schedule[currentMatchday]) {
        match->simulate();

        // Display match result
        std::cout << match->getMatchEvents()[0] << std::endl;
        if (!match->getMatchEvents().empty()) {
            std::cout << match->getMatchEvents().back() << std::endl;
        }
    }

    // Increment matchday counter
    currentMatchday++;

    std::cout << "Kolejka " << currentMatchday << " zakończona." << std::endl;
}

bool LeagueEngine::isLeagueFinished() const {
    return currentMatchday >= schedule.size();
}

void LeagueEngine::sortTable() {
    std::sort(teams.begin(), teams.end(),
        [](const std::shared_ptr<Team>& a, const std::shared_ptr<Team>& b) {
            // Sort by points descending
            if (a->getPoints() != b->getPoints()) {
                return a->getPoints() > b->getPoints();
            }
            // If points are equal, sort by goal difference descending
            int diffA = a->getGoalsScored() - a->getGoalsConceded();
            int diffB = b->getGoalsScored() - b->getGoalsConceded();
            if (diffA != diffB) {
                return diffA > diffB;
            }
            // If goal difference is equal, sort by goals scored descending
            return a->getGoalsScored() > b->getGoalsScored();
        });
}

void LeagueEngine::displayTable() const {
    std::cout << "\n╔════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    TABELA LIGOWA - KOLEJKA " << std::setw(2) << currentMatchday
              << "                      ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Poz │ Drużyna              │  Pkt │ M  │ W  │ R  │ P  │ +  │ -  │" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════╣" << std::endl;

    int position = 1;
    for (const auto& team : teams) {
        std::cout << "║ " << std::setw(2) << position << "  │ "
                  << std::setw(20) << std::left << team->getName() << "│ "
                  << std::setw(4) << std::right << team->getPoints() << "│ "
                  << std::setw(3) << team->getMatchesPlayed() << "│ "
                  << std::setw(3) << team->getWins() << "│ "
                  << std::setw(3) << team->getDraws() << "│ "
                  << std::setw(3) << team->getLosses() << "│ "
                  << std::setw(3) << team->getGoalsScored() << "│ "
                  << std::setw(3) << team->getGoalsConceded() << "│" << std::endl;
        position++;
    }

    std::cout << "╚════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void LeagueEngine::exportToFile(const std::string& filename) const {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Błąd: Nie można otworzyć pliku " << filename << " do zapisu!" << std::endl;
        return;
    }

    // Write header
    outFile << "================================ TABELA LIGOWA ================================" << std::endl;
    outFile << "Kolejka: " << currentMatchday << " / " << schedule.size() << std::endl;
    outFile << "Data: 2026-03-16" << std::endl;
    outFile << std::endl;

    // Write table
    outFile << "Poz │ Drużyna              │  Pkt │ M  │ W  │ R  │ P  │ +  │ -  │ Różnica" << std::endl;
    outFile << "────┼────────────────────┼──────┼────┼────┼────┼────┼────┼────┼────────" << std::endl;

    int position = 1;
    for (const auto& team : teams) {
        int goalDiff = team->getGoalsScored() - team->getGoalsConceded();
        outFile << std::setw(3) << position << " │ "
                << std::setw(20) << std::left << team->getName() << "│ "
                << std::setw(5) << std::right << team->getPoints() << "│ "
                << std::setw(3) << team->getMatchesPlayed() << "│ "
                << std::setw(3) << team->getWins() << "│ "
                << std::setw(3) << team->getDraws() << "│ "
                << std::setw(3) << team->getLosses() << "│ "
                << std::setw(3) << team->getGoalsScored() << "│ "
                << std::setw(3) << team->getGoalsConceded() << "│ "
                << std::setw(4) << (goalDiff >= 0 ? "+" : "") << goalDiff << std::endl;
        position++;
    }

    outFile << std::endl;
    outFile << "================================ HISTORIA MECZÓW ================================" << std::endl;
    outFile << std::endl;

    // Write match history
    int matchdayNum = 1;
    for (const auto& matchday : schedule) {
        outFile << "--- KOLEJKA " << matchdayNum << " ---" << std::endl;

        for (const auto& match : matchday) {
            const auto& events = match->getMatchEvents();
            if (!events.empty()) {
                // Write match info
                outFile << match->getMatchEvents()[0] << std::endl;
                if (events.size() > 1) {
                    outFile << "Wynik: " << match->getHomeGoals() << " - " << match->getAwayGoals() << std::endl;
                }

                // Write match events
                for (size_t i = 1; i < events.size(); ++i) {
                    outFile << "  • " << events[i] << std::endl;
                }
            }
        }

        outFile << std::endl;
        matchdayNum++;
    }

    outFile << "================================ KONIEC RAPORTU ================================" << std::endl;
    outFile.close();

    std::cout << "Raport zapisany do pliku: " << filename << std::endl;
}

const std::vector<std::shared_ptr<Team>>& LeagueEngine::getTeams() const {
    return teams;
}

std::shared_ptr<Team> LeagueEngine::getPlayerTeam() const {
    return playerTeam;
}

const std::vector<std::vector<std::shared_ptr<Match>>>& LeagueEngine::getSchedule() const {
    return schedule;
}

int LeagueEngine::getCurrentMatchday() const {
    return currentMatchday;
}

int LeagueEngine::getTotalMatchdays() const {
    return schedule.size();
}

