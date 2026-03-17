#ifndef FIFA26_SIMULATOR_GOALKEEPER_H
#define FIFA26_SIMULATOR_GOALKEEPER_H

#include "Player.h"
#include "Statistic.h"
#include <string>

/**
 * @file Goalkeeper.h
 * @brief Deklaracja klasy bramkarza.
 */

/**
 * @brief Reprezentuje bramkarza.
 */
class Goalkeeper : public Player {
private:
    Statistic<int> defendingSkills;

public:
    /** @brief Tworzy bramkarza. */
    Goalkeeper(std::string _name, std::string _surname, double _condition, int _overall, int _defendingSkills);

    /** @copydoc Player::calculateGoalChance */
    double calculateGoalChance() const override;

    /** @return Wartosc umiejetnosci defensywnych. */
    int getDefendingSkills() const;
    /** @brief Ustawia umiejetnosci defensywne. */
    void setDefendingSkills(int value);
};

#endif