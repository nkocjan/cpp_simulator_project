#ifndef FIFA26_SIMULATOR_MIDFIELDER_H
#define FIFA26_SIMULATOR_MIDFIELDER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

/**
 * @file Midfielder.h
 * @brief Deklaracja klasy pomocnika.
 */

/**
 * @brief Reprezentuje pomocnika.
 */
class Midfielder : public Player {
private:
    Statistic<int> passingVision;

public:
    /** @brief Tworzy pomocnika. */
    Midfielder(std::string _name, std::string _surname, double _condition, int _overall, int _passingVision);

    /** @copydoc Player::calculateGoalChance */
    double calculateGoalChance() const override;
    /** @copydoc Player::getNaturalPosition */
    PlayerPosition getNaturalPosition() const override;

    /** @return Wartosc passing vision. */
    int getPassingVision() const;
    /** @brief Ustawia passing vision. */
    void setPassingVision(int value);
};

#endif
