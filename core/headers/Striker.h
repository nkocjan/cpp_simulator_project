#ifndef FIFA26_SIMULATOR_STRIKER_H
#define FIFA26_SIMULATOR_STRIKER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

class Striker : public Player {
private:
    Statistic<int> shootingFinishing;

public:
    Striker(std::string _name, std::string _surname, double _condition, int _overall, int _shootingFinishing);

    double calculateGoalChance() const override;

    int getShootingFinishing() const;
    void setShootingFinishing(int value);
};

#endif