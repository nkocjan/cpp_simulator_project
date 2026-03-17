#ifndef FIFA26_SIMULATOR_DEFENDER_H
#define FIFA26_SIMULATOR_DEFENDER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

class Defender : public Player {
private:
    Statistic<int> defendingSkills;

public:
    Defender(std::string _name, std::string _surname, double _condition, int _overall, int _defendingSkills);

    double calculateGoalChance() const override;

    int getDefendingSkills() const;
    void setDefendingSkills(int value);
};

#endif