#pragma once
#include <string>
#include "Statistic.h"

class Player {
protected:
    std::string name;
    std::string surname;
    Statistic<double> condition;
    Statistic<int> overall;

    // Player statistics
    Statistic<int> goals;
    Statistic<int> assists;
    Statistic<int> yellowCards;
    Statistic<int> redCards;
    Statistic<int> minutesPlayed;
    Statistic<double> morale;

    // Injury status
    int injuryDuration;

public:
    Player(std::string _name, std::string _surname, double _condition, int _overall);
    
    virtual ~Player() = default;

    virtual double calculateGoalChance() const = 0;

    std::string getName() const;
    std::string getSurname() const;

    // Getters for statistics
    [[nodiscard]] double getCondition() const;
    [[nodiscard]] int getOverall() const;
    [[nodiscard]] int getGoals() const;
    [[nodiscard]] int getAssists() const;
    [[nodiscard]] int getYellowCards() const;
    [[nodiscard]] int getRedCards() const;
    [[nodiscard]] int getMinutesPlayed() const;
    [[nodiscard]] double getMorale() const;
    [[nodiscard]] int getInjuryDuration() const;

    // Setters for statistics
    void setCondition(double value);
    void setOverall(int value);
    void addGoal();
    void addAssist();
    void addYellowCard();
    void addRedCard();
    void addMinutesPlayed(int minutes);
    void setMorale(double value);
    void setInjuryDuration(int duration);

    // Statistics objects access
    Statistic<double>& getConditionStat() { return condition; }
    Statistic<int>& getOverallStat() { return overall; }
};
