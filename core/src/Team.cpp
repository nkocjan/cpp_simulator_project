#include "../headers/Team.h"

#include <algorithm>
#include <set>
#include <utility>

namespace {
size_t toLineupIndex(PlayerPosition position) {
    switch (position) {
    case PlayerPosition::Goalkeeper: return 0;
    case PlayerPosition::Defender: return 1;
    case PlayerPosition::Midfielder: return 2;
    case PlayerPosition::Striker: return 3;
    }
    return 0;
}

std::array<PlayerPosition, 4> requiredPositions() {
    return {PlayerPosition::Goalkeeper, PlayerPosition::Defender, PlayerPosition::Midfielder, PlayerPosition::Striker};
}

double offPositionPenalty(PlayerPosition expected, PlayerPosition natural) {
    return expected == natural ? 1.0 : 0.8;
}

double conditionMultiplier(double condition) {
    const double normalized = std::clamp(condition / 100.0, 0.0, 1.0);
    return 0.5 + (0.5 * normalized);
}

double moraleMultiplier(double morale) {
    const double normalized = std::clamp(morale / 100.0, 0.0, 1.0);
    return 0.8 + (0.2 * normalized);
}

}

Team::Team(std::string _name)
    : name(std::move(_name)),
      points("Points", 0, 0, 999999),
      goalsScored("Goals Scored", 0, 0, 999),
      goalsConceded("Goals Conceded", 0, 0, 999),
      matchesPlayed("Matches Played", 0, 0, 9999),
      wins("Wins", 0, 0, 9999),
      draws("Draws", 0, 0, 9999),
      losses("Losses", 0, 0, 9999),
      startingLineup{} {}

void Team::addPlayer(std::shared_ptr<Player> player) {
    if (!player) {
        return;
    }

    players.push_back(std::move(player));

    const auto index = toLineupIndex(players.back()->getNaturalPosition());
    if (!startingLineup[index]) {
        startingLineup[index] = players.back();
    }
}

double Team::calculateTeamStrength() const {
    if (players.empty()) {
        return 0.0;
    }

    double totalStrength = 0.0;
    for (const auto& player : players) {
        totalStrength += player->calculateGoalChance();
    }

    return totalStrength / static_cast<double>(players.size());
}

double Team::calculateLineupStrength(const std::vector<std::shared_ptr<Player>>& lineup) const {
    if (lineup.empty()) {
        return 0.0;
    }

    const auto positions = requiredPositions();
    double totalStrength = 0.0;

    for (size_t i = 0; i < lineup.size(); ++i) {
        const auto& player = lineup[i];
        if (!player) {
            continue;
        }

        const PlayerPosition expectedPosition = positions[std::min(i, positions.size() - 1)];
        const double positionalPenalty = offPositionPenalty(expectedPosition, player->getNaturalPosition());
        const double fatiguePenalty = conditionMultiplier(player->getCondition());
        const double psychePenalty = moraleMultiplier(player->getMorale());

        totalStrength += player->calculateGoalChance() * positionalPenalty * fatiguePenalty * psychePenalty;
    }

    return totalStrength / static_cast<double>(lineup.size());
}

std::string Team::getName() const {
    return name;
}

int Team::getPoints() const {
    return points.getValue();
}

int Team::getGoalsScored() const {
    return goalsScored.getValue();
}

int Team::getGoalsConceded() const {
    return goalsConceded.getValue();
}

int Team::getMatchesPlayed() const {
    return matchesPlayed.getValue();
}

int Team::getWins() const {
    return wins.getValue();
}

int Team::getDraws() const {
    return draws.getValue();
}

int Team::getLosses() const {
    return losses.getValue();
}

void Team::addPoints(int p) {
    points.add(p);
}

void Team::addGoalScored(int g) {
    goalsScored.add(g);
}

void Team::addGoalConceded(int g) {
    goalsConceded.add(g);
}

void Team::addMatch() {
    matchesPlayed.add(1);
}

void Team::addWin() {
    wins.add(1);
}

void Team::addDraw() {
    draws.add(1);
}

void Team::addLoss() {
    losses.add(1);
}

const std::vector<std::shared_ptr<Player>>& Team::getPlayers() const {
    return players;
}

bool Team::hasPlayer(const std::shared_ptr<Player>& player) const {
    if (!player) {
        return false;
    }
    return std::find(players.begin(), players.end(), player) != players.end();
}

bool Team::removePlayer(const std::shared_ptr<Player>& player) {
    if (!player) {
        return false;
    }

    const auto it = std::find(players.begin(), players.end(), player);
    if (it == players.end()) {
        return false;
    }

    for (auto& starter : startingLineup) {
        if (starter == player) {
            starter.reset();
        }
    }

    players.erase(it);
    return true;
}

void Team::setDefaultLineup() {
    startingLineup = {};

    auto pickBestByPosition = [this](PlayerPosition position) -> std::shared_ptr<Player> {
        std::shared_ptr<Player> best;
        int bestOverall = -1;
        for (const auto& player : players) {
            if (!player || !player->isAvailable()) {
                continue;
            }
            if (player->getNaturalPosition() != position) {
                continue;
            }
            if (player->getOverall() > bestOverall) {
                bestOverall = player->getOverall();
                best = player;
            }
        }
        return best;
    };

    for (const auto position : requiredPositions()) {
        const auto index = toLineupIndex(position);
        startingLineup[index] = pickBestByPosition(position);
    }
}

bool Team::setStarter(PlayerPosition position, const std::shared_ptr<Player>& player) {
    if (!player) {
        return false;
    }

    const auto it = std::find(players.begin(), players.end(), player);
    if (it == players.end()) {
        return false;
    }

    startingLineup[toLineupIndex(position)] = player;
    return true;
}

std::shared_ptr<Player> Team::getStarter(PlayerPosition position) const {
    return startingLineup[toLineupIndex(position)];
}

std::vector<std::shared_ptr<Player>> Team::getBenchPlayers() const {
    std::set<const Player*> starters;
    for (const auto& p : startingLineup) {
        if (p) {
            starters.insert(p.get());
        }
    }

    std::vector<std::shared_ptr<Player>> bench;
    for (const auto& p : players) {
        if (p && starters.find(p.get()) == starters.end()) {
            bench.push_back(p);
        }
    }
    return bench;
}

std::vector<std::shared_ptr<Player>> Team::buildMatchLineup(bool& walkover) const {
    walkover = false;

    std::vector<std::shared_ptr<Player>> lineup;
    lineup.reserve(4);
    std::set<const Player*> used;

    auto pickBestAvailable = [this, &used](PlayerPosition preferredPosition, bool strictPosition) -> std::shared_ptr<Player> {
        std::shared_ptr<Player> best;
        int bestOverall = -1;

        for (const auto& candidate : players) {
            if (!candidate || !candidate->isAvailable()) {
                continue;
            }
            if (used.find(candidate.get()) != used.end()) {
                continue;
            }
            if (strictPosition && candidate->getNaturalPosition() != preferredPosition) {
                continue;
            }
            if (candidate->getOverall() > bestOverall) {
                bestOverall = candidate->getOverall();
                best = candidate;
            }
        }
        return best;
    };

    for (const auto position : requiredPositions()) {
        const auto index = toLineupIndex(position);
        std::shared_ptr<Player> selected = startingLineup[index];

        if (!selected || !selected->isAvailable() || used.find(selected.get()) != used.end()) {
            selected = pickBestAvailable(position, true);
            if (!selected) {
                selected = pickBestAvailable(position, false);
            }
        }

        if (!selected) {
            walkover = true;
            return {};
        }

        used.insert(selected.get());
        lineup.push_back(selected);
    }

    return lineup;
}

void Team::recoverConditionForNonPlaying(const std::vector<std::shared_ptr<Player>>& playedPlayers,
                                         double amount) {
    std::set<const Player*> played;
    for (const auto& p : playedPlayers) {
        if (p) {
            played.insert(p.get());
        }
    }

    for (const auto& player : players) {
        if (!player) {
            continue;
        }
        if (played.find(player.get()) == played.end()) {
            player->getConditionStat().add(amount);
        }
    }
}

void Team::decrementUnavailablePlayers() {
    for (const auto& player : players) {
        if (!player) {
            continue;
        }
        player->decrementInjuryDuration();
        player->decrementSuspensionDuration();
    }
}
