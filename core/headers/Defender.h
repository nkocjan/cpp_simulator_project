#ifndef FIFA26_SIMULATOR_DEFENDER_H
#define FIFA26_SIMULATOR_DEFENDER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

/**
 * @file Defender.h
 * @brief Deklaracja klasy obroncy.
 */

/**
 * @brief Reprezentuje obronce.
 */
class Defender : public Player {
    Statistic<int> defendingSkills;

public:
    /** @brief Tworzy obronce. */
    Defender(std::string _name, std::string _surname, double _condition, int _overall, int _defendingSkills);

    /** @copydoc Player::calculateGoalChance */
    double calculateGoalChance() const override;
    /** @copydoc Player::getNaturalPosition */
    PlayerPosition getNaturalPosition() const override;

    /** @return Wartosc umiejetnosci defensywnych. */
    int getDefendingSkills() const;
    /** @brief Ustawia umiejetnosci defensywne. */
    void setDefendingSkills(int value);
};

#endif
