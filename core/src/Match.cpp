#include "../headers/Match.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <set>

static std::mt19937 g_gen(std::random_device{}());

Match::Match(std::shared_ptr<Team> home, std::shared_ptr<Team> away, int randomness)
    : homeTeam(std::move(home)), awayTeam(std::move(away)),
      homeGoals("Home Goals", 0, 0, 20),
      awayGoals("Away Goals", 0, 0, 20),
      isFinished(false),
      randomnessPercent(std::clamp(randomness, 0, 100)),
      weekendSlot("Sobota 15:00") {}

void Match::simulate() {
    if (isFinished) {
        return;
    }

    homeScorers.clear();
    awayScorers.clear();

    bool homeWalkover = false;
    bool awayWalkover = false;
    homeMatchPlayers = homeTeam->buildMatchLineup(homeWalkover);
    awayMatchPlayers = awayTeam->buildMatchLineup(awayWalkover);

    matchEvents.push_back("Mecz pomiedzy " + homeTeam->getName() + " a " + awayTeam->getName() + " rozpoczal sie!");

    if (homeWalkover || awayWalkover) {
        int homeGoalCount = 0;
        int awayGoalCount = 0;

        if (homeWalkover && !awayWalkover) {
            awayGoalCount = 3;
            matchEvents.push_back("Walkower: " + homeTeam->getName() + " nie miala 4 dostepnych zawodnikow.");
        } else if (!homeWalkover && awayWalkover) {
            homeGoalCount = 3;
            matchEvents.push_back("Walkower: " + awayTeam->getName() + " nie miala 4 dostepnych zawodnikow.");
        } else {
            matchEvents.push_back("Obie druzyny bez pelnego skladu - mecz zakonczony walkowerem 0:0.");
        }

        homeGoals.setValue(homeGoalCount);
        awayGoals.setValue(awayGoalCount);

        homeTeam->addMatch();
        awayTeam->addMatch();
        homeTeam->addGoalScored(homeGoalCount);
        homeTeam->addGoalConceded(awayGoalCount);
        awayTeam->addGoalScored(awayGoalCount);
        awayTeam->addGoalConceded(homeGoalCount);

        if (homeGoalCount > awayGoalCount) {
            homeTeam->addWin();
            awayTeam->addLoss();
            homeTeam->addPoints(3);
        } else if (awayGoalCount > homeGoalCount) {
            awayTeam->addWin();
            homeTeam->addLoss();
            awayTeam->addPoints(3);
        } else {
            homeTeam->addDraw();
            awayTeam->addDraw();
            homeTeam->addPoints(1);
            awayTeam->addPoints(1);
        }

        homeTeam->recoverConditionForNonPlaying(homeMatchPlayers, 20.0);
        awayTeam->recoverConditionForNonPlaying(awayMatchPlayers, 20.0);

        applyPostMatchMorale(homeGoalCount, awayGoalCount);

        matchEvents.push_back("Wynik koncowy: " + std::to_string(homeGoalCount) + ":" + std::to_string(awayGoalCount));
        isFinished = true;
        return;
    }

    const double homeStrength = homeTeam->calculateLineupStrength(homeMatchPlayers);
    const double awayStrength = awayTeam->calculateLineupStrength(awayMatchPlayers);

    std::uniform_int_distribution<> randomGoals(0, 3);
    const double randomnessWeight = static_cast<double>(randomnessPercent) / 100.0;

    const double deterministicHome = std::max(0.0, 1.2 + (homeStrength - awayStrength) / 30.0);
    const double deterministicAway = std::max(0.0, 1.0 + (awayStrength - homeStrength) / 30.0);

    int homeGoalCount = static_cast<int>(std::round((1.0 - randomnessWeight) * deterministicHome + randomnessWeight * randomGoals(g_gen)));
    int awayGoalCount = static_cast<int>(std::round((1.0 - randomnessWeight) * deterministicAway + randomnessWeight * randomGoals(g_gen)));

    homeGoalCount = std::clamp(homeGoalCount, 0, 20);
    awayGoalCount = std::clamp(awayGoalCount, 0, 20);

    homeGoals.setValue(homeGoalCount);
    awayGoals.setValue(awayGoalCount);

    assignGoalsToPlayers(homeMatchPlayers, homeGoalCount, homeScorers);
    assignGoalsToPlayers(awayMatchPlayers, awayGoalCount, awayScorers);

    simulateCards();
    simulateInjuries();

    homeTeam->addMatch();
    awayTeam->addMatch();

    homeTeam->addGoalScored(homeGoalCount);
    homeTeam->addGoalConceded(awayGoalCount);
    awayTeam->addGoalScored(awayGoalCount);
    awayTeam->addGoalConceded(homeGoalCount);

    const int conditionDrop = 8 + (randomnessPercent / 4);
    const int matchMinutes = 90;
    for (auto& player : homeMatchPlayers) {
        player->addMinutesPlayed(matchMinutes);
        player->getConditionStat().subtract(static_cast<double>(conditionDrop));
    }
    for (auto& player : awayMatchPlayers) {
        player->addMinutesPlayed(matchMinutes);
        player->getConditionStat().subtract(static_cast<double>(conditionDrop));
    }

    homeTeam->recoverConditionForNonPlaying(homeMatchPlayers, 20.0);
    awayTeam->recoverConditionForNonPlaying(awayMatchPlayers, 20.0);

    applyPostMatchMorale(homeGoalCount, awayGoalCount);

    matchEvents.push_back("Wynik koncowy: " + std::to_string(homeGoalCount) + ":" + std::to_string(awayGoalCount));

    if (homeGoalCount > awayGoalCount) {
        homeTeam->addWin();
        awayTeam->addLoss();
        homeTeam->addPoints(3);
        matchEvents.push_back("Zwyciestwo " + homeTeam->getName() + "!");
    } else if (awayGoalCount > homeGoalCount) {
        awayTeam->addWin();
        homeTeam->addLoss();
        awayTeam->addPoints(3);
        matchEvents.push_back("Zwyciestwo " + awayTeam->getName() + "!");
    } else {
        homeTeam->addDraw();
        awayTeam->addDraw();
        homeTeam->addPoints(1);
        awayTeam->addPoints(1);
        matchEvents.emplace_back("Remis!");
    }

    isFinished = true;
}

void Match::assignGoalsToPlayers(const std::vector<std::shared_ptr<Player>>& lineup,
                                 int goalCount,
                                 std::set<const Player*>& scorers) {
    if (goalCount == 0 || lineup.empty()) {
        return;
    }

    std::uniform_int_distribution<> playerDis(0, static_cast<int>(lineup.size()) - 1);

    for (int i = 0; i < goalCount; ++i) {
        const int playerIndex = playerDis(g_gen);
        const auto& player = lineup[static_cast<size_t>(playerIndex)];
        player->addGoal();
        scorers.insert(player.get());
        matchEvents.push_back("GOOOL! " + player->getName() + " " + player->getSurname() + " strzela!");
    }
}

void Match::simulateCards() {
    std::uniform_int_distribution<> dis(0, 100);

    const int yellowChance = std::clamp((20 * randomnessPercent) / 100, 0, 100);
    const int redChance = std::clamp((5 * randomnessPercent) / 100, 0, 100);

    if (dis(g_gen) < yellowChance && !homeMatchPlayers.empty()) {
        assignCardsToPlayers(homeMatchPlayers, 1, true);
    }
    if (dis(g_gen) < yellowChance && !awayMatchPlayers.empty()) {
        assignCardsToPlayers(awayMatchPlayers, 1, true);
    }

    if (dis(g_gen) < redChance && !homeMatchPlayers.empty()) {
        assignCardsToPlayers(homeMatchPlayers, 1, false);
    }
    if (dis(g_gen) < redChance && !awayMatchPlayers.empty()) {
        assignCardsToPlayers(awayMatchPlayers, 1, false);
    }
}

void Match::assignCardsToPlayers(const std::vector<std::shared_ptr<Player>>& lineup, int cardCount, bool yellowCard) {
    if (lineup.empty()) {
        return;
    }

    std::uniform_int_distribution<> playerDis(0, static_cast<int>(lineup.size()) - 1);

    for (int i = 0; i < cardCount; ++i) {
        const int playerIndex = playerDis(g_gen);
        const auto& player = lineup[static_cast<size_t>(playerIndex)];

        if (yellowCard) {
            player->addYellowCard();
            matchEvents.push_back("ZOLTA KARTKA! " + player->getName() + " " + player->getSurname());
        } else {
            player->addRedCard();
            player->setSuspensionDuration(std::max(player->getSuspensionDuration(), 1));
            matchEvents.push_back("CZERWONA KARTKA! " + player->getName() + " " + player->getSurname());
        }
    }
}

void Match::simulateInjuries() {
    std::uniform_int_distribution<> dis(0, 1000);
    const int baseInjuryChance = std::clamp((20 * randomnessPercent) / 100, 0, 1000);

    auto injuryChanceFor = [baseInjuryChance](const std::shared_ptr<Player>& player) {
        const double conditionFactor = std::clamp((100.0 - player->getCondition()) / 100.0, 0.0, 1.0);
        const int extra = static_cast<int>(std::round(baseInjuryChance * 2.0 * conditionFactor));
        return std::clamp(baseInjuryChance + extra, 0, 1000);
    };

    for (auto& player : homeMatchPlayers) {
        if (dis(g_gen) < injuryChanceFor(player)) {
            player->setInjuryDuration(std::max(player->getInjuryDuration(), 2));
            matchEvents.push_back("KONTUZJA! " + player->getName() + " " + player->getSurname() + " pauzuje 2 mecze!");
        }
    }
    for (auto& player : awayMatchPlayers) {
        if (dis(g_gen) < injuryChanceFor(player)) {
            player->setInjuryDuration(std::max(player->getInjuryDuration(), 2));
            matchEvents.push_back("KONTUZJA! " + player->getName() + " " + player->getSurname() + " pauzuje 2 mecze!");
        }
    }
}

void Match::applyTeamMorale(const std::shared_ptr<Team>& team,
                            const std::vector<std::shared_ptr<Player>>& playedPlayers,
                            const std::set<const Player*>& scorers,
                            bool won,
                            bool lost) {
    if (!team) {
        return;
    }

    std::set<const Player*> played;
    for (const auto& p : playedPlayers) {
        if (p) {
            played.insert(p.get());
        }
    }

    for (const auto& player : team->getPlayers()) {
        if (!player) {
            continue;
        }

        const bool didPlay = played.find(player.get()) != played.end();
        const bool scored = scorers.find(player.get()) != scorers.end();

        if (didPlay && won) {
            player->adjustMorale(15.0);
        }
        if (!didPlay && won) {
            player->adjustMorale(10.0);
        }
        if (!didPlay && lost) {
            player->adjustMorale(-25.0);
        }
        if (didPlay && scored) {
            player->adjustMorale(25.0);
        }
    }
}

void Match::applyPostMatchMorale(int homeGoalCount, int awayGoalCount) {
    const bool homeWon = homeGoalCount > awayGoalCount;
    const bool awayWon = awayGoalCount > homeGoalCount;

    applyTeamMorale(homeTeam, homeMatchPlayers, homeScorers, homeWon, awayWon);
    applyTeamMorale(awayTeam, awayMatchPlayers, awayScorers, awayWon, homeWon);
}

const std::string& Match::getWeekendSlot() const {
    return weekendSlot;
}

bool Match::setWeekendSlot(const std::string& slot) {
    if (isFinished || slot.empty()) {
        return false;
    }
    weekendSlot = slot;
    return true;
}

std::shared_ptr<Team> Match::getHomeTeam() const {
    return homeTeam;
}

std::shared_ptr<Team> Match::getAwayTeam() const {
    return awayTeam;
}

int Match::getHomeGoals() const {
    return homeGoals.getValue();
}

int Match::getAwayGoals() const {
    return awayGoals.getValue();
}

bool Match::getIsFinished() const {
    return isFinished;
}

const std::vector<std::string>& Match::getMatchEvents() const {
    return matchEvents;
}
