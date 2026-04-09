#ifndef FIFA26_SIMULATOR_STRIKER_H
#define FIFA26_SIMULATOR_STRIKER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

/**
 * @file Striker.h
 * @brief Deklaracja klasy napastnika.
 */

/**
 * @brief Reprezentuje napastnika.
 */
class Striker : public Player {
private:
    Statistic<int> shootingFinishing;

public:
    /**
     * @brief Tworzy napastnika.
     */
    Striker(std::string _name, std::string _surname, double _condition, int _overall, int _shootingFinishing);

    /** @copydoc Player::calculateGoalChance */
    double calculateGoalChance() const override;
    /** @copydoc Player::getNaturalPosition */
    PlayerPosition getNaturalPosition() const override;

    /** @return Wartosc wykanczania akcji. */
    int getShootingFinishing() const;
    /** @brief Ustawia wykanczanie akcji. */
    void setShootingFinishing(int value);
};

#endif