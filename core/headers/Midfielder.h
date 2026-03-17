#ifndef FIFA26_SIMULATOR_MIDFIELDER_H
#define FIFA26_SIMULATOR_MIDFIELDER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

class Midfielder : public Player {
private:
    Statistic<int> passingVision;

public:
    Midfielder(std::string _name, std::string _surname, double _condition, int _overall, int _passingVision);

    double calculateGoalChance() const override;

    int getPassingVision() const;
    void setPassingVision(int value);
};

#endif //FIFA26_SIMULATOR_MIDFIELDER_H
