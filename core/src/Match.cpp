#include "../headers/Match.h"
#include <iostream>
#include <random>

static std::mt19937 g_gen(std::random_device{}());

Match::Match(std::shared_ptr<Team> home, std::shared_ptr<Team> away)
    : homeTeam(std::move(home)), awayTeam(std::move(away)),
      homeGoals("Home Goals", 0, 0, 20),
      awayGoals("Away Goals", 0, 0, 20),
      isFinished(false) {}

void Match::simulate() {
    if (isFinished) return;

    // Random engine setup
    std::uniform_int_distribution<> dis(0, 10);

    double homeStrength = homeTeam->calculateTeamStrength();
    double awayStrength = awayTeam->calculateTeamStrength();

    matchEvents.push_back("Mecz pomiędzy " + homeTeam->getName() + " a " + awayTeam->getName() + " rozpoczął się!");

    // Simulate goals based on team strength
    int homeGoalCount = (dis(g_gen) % 4) + (homeStrength > awayStrength ? 1 : 0);
    int awayGoalCount = (dis(g_gen) % 4) + (awayStrength > homeStrength ? 1 : 0);

    homeGoals.setValue(homeGoalCount);
    awayGoals.setValue(awayGoalCount);

    // Assign goals to specific players
    assignGoalsToPlayers(homeTeam, homeGoalCount);
    assignGoalsToPlayers(awayTeam, awayGoalCount);

    // Simulate cards
    simulateCards();

    // Simulate injuries
    simulateInjuries();

    // Update team statistics
    homeTeam->addMatch();
    awayTeam->addMatch();

    homeTeam->addGoalScored(homeGoalCount);
    homeTeam->addGoalConceded(awayGoalCount);
    awayTeam->addGoalScored(awayGoalCount);
    awayTeam->addGoalConceded(homeGoalCount);

    // Update all players' minutes
    const int MATCH_MINUTES = 90;
    for (auto& player : homeTeam->getPlayers()) {
        player->addMinutesPlayed(MATCH_MINUTES);
        player->getConditionStat().subtract(15.0); // Condition decreases after match
    }
    for (auto& player : awayTeam->getPlayers()) {
        player->addMinutesPlayed(MATCH_MINUTES);
        player->getConditionStat().subtract(15.0);
    }

    matchEvents.push_back("Wynik końcowy: " + std::to_string(homeGoalCount) + ":" + std::to_string(awayGoalCount));

    // Award points and determine winner
    if (homeGoalCount > awayGoalCount) {
        homeTeam->addWin();
        awayTeam->addLoss();
        homeTeam->addPoints(3);
        matchEvents.push_back("Zwycięstwo " + homeTeam->getName() + "!");
    } else if (awayGoalCount > homeGoalCount) {
        awayTeam->addWin();
        homeTeam->addLoss();
        awayTeam->addPoints(3);
        matchEvents.push_back("Zwycięstwo " + awayTeam->getName() + "!");
    } else {
        homeTeam->addDraw();
        awayTeam->addDraw();
        homeTeam->addPoints(1);
        awayTeam->addPoints(1);
        matchEvents.emplace_back("Remis!");
    }

    isFinished = true;
}

void Match::assignGoalsToPlayers(const std::shared_ptr<Team>& team, int goalCount) {
    if (goalCount == 0 || team->getPlayers().empty()) return;

    std::uniform_int_distribution<> playerDis(0, static_cast<int>(team->getPlayers().size()) - 1);

    for (int i = 0; i < goalCount; ++i) {
        int playerIndex = playerDis(g_gen);
        auto player = team->getPlayers()[playerIndex];
        player->addGoal();
        matchEvents.push_back("GOOOOOOL! " + player->getName() + " " + player->getSurname() + " strzela!");
    }
}

void Match::simulateCards() {
    std::uniform_int_distribution<> dis(0, 100);

    // 20% chance for yellow cards
    if (dis(g_gen) < 20 && !homeTeam->getPlayers().empty()) {
        assignCardsToPlayers(homeTeam, 1, true);
    }
    if (dis(g_gen) < 20 && !awayTeam->getPlayers().empty()) {
        assignCardsToPlayers(awayTeam, 1, true);
    }

    // 5% chance for red cards
    if (dis(g_gen) < 5 && !homeTeam->getPlayers().empty()) {
        assignCardsToPlayers(homeTeam, 1, false);
    }
    if (dis(g_gen) < 5 && !awayTeam->getPlayers().empty()) {
        assignCardsToPlayers(awayTeam, 1, false);
    }
}

void Match::assignCardsToPlayers(const std::shared_ptr<Team>& team, int cardCount, bool yellowCard) {
    if (team->getPlayers().empty()) return;

    std::uniform_int_distribution<> playerDis(0, static_cast<int>(team->getPlayers().size()) - 1);

    for (int i = 0; i < cardCount; ++i) {
        int playerIndex = playerDis(g_gen);
        auto player = team->getPlayers()[playerIndex];

        if (yellowCard) {
            player->addYellowCard();
            matchEvents.push_back("ŻÓŁTA KARTKA! " + player->getName() + " " + player->getSurname());
        } else {
            player->addRedCard();
            matchEvents.push_back("CZERWONA KARTKA! " + player->getName() + " " + player->getSurname());
            player->setInjuryDuration(3); // Red card = 3 match ban
        }
    }
}

void Match::simulateInjuries() {
    std::uniform_int_distribution<> dis(0, 1000);

    // 2% chance for injury to any player
    for (auto& player : homeTeam->getPlayers()) {
        if (dis(g_gen) < 20) {
            player->setInjuryDuration(2);
            matchEvents.push_back("KONTUZJA! " + player->getName() + " " + player->getSurname() + " pauzuje 2 mecze!");
        }
    }
    for (auto& player : awayTeam->getPlayers()) {
        if (dis(g_gen) < 20) {
            player->setInjuryDuration(2);
            matchEvents.push_back("KONTUZJA! " + player->getName() + " " + player->getSurname() + " pauzuje 2 mecze!");
        }
    }
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
