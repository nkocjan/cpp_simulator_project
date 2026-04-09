#pragma once
#include <string>
#include "Statistic.h"

/**
 * @file Player.h
 * @brief Deklaracja abstrakcyjnej klasy bazowej pilkarza.
 */

/** @brief Podstawowe pozycje boiskowe obslugiwane w symulacji. */
enum class PlayerPosition {
    Goalkeeper,
    Defender,
    Midfielder,
    Striker
};

/**
 * @brief Bazowa klasa reprezentujaca pilkarza.
 *
 * Klasa przechowuje dane osobowe, statystyki oraz status dostepnosci.
 */
class Player {
protected:
    std::string name;
    std::string surname;
    Statistic<double> condition;
    Statistic<int> overall;

    Statistic<int> goals;
    Statistic<int> assists;
    Statistic<int> yellowCards;
    Statistic<int> redCards;
    Statistic<int> minutesPlayed;
    Statistic<double> morale;

    int injuryDuration;
    int suspensionDuration;

public:
    /**
     * @brief Tworzy pilkarza z danymi poczatkowymi.
     * @param _name Imie.
     * @param _surname Nazwisko.
     * @param _condition Kondycja startowa.
     * @param _overall Ocena ogolna startowa.
     */
    Player(std::string _name, std::string _surname, double _condition, int _overall);

    virtual ~Player() = default;

    /**
     * @brief Oblicza szanse udanej akcji ofensywnej dla roli zawodnika.
     * @return Wskaznik szansy wyliczony dla danej pozycji.
     */
    virtual double calculateGoalChance() const = 0;
    /** @return Naturalna pozycja zawodnika. */
    virtual PlayerPosition getNaturalPosition() const = 0;

    /** @return Imie pilkarza. */
    std::string getName() const;
    /** @return Nazwisko pilkarza. */
    std::string getSurname() const;

    /** @return Aktualna kondycja. */
     double getCondition() const;
    /** @return Aktualny overall. */
     int getOverall() const;
    /** @return Liczba goli. */
     int getGoals() const;
    /** @return Liczba asyst. */
     int getAssists() const;
    /** @return Liczba zoltych kartek. */
     int getYellowCards() const;
    /** @return Liczba czerwonych kartek. */
     int getRedCards() const;
    /** @return Liczba rozegranych minut. */
     int getMinutesPlayed() const;
    /** @return Aktualne morale. */
     double getMorale() const;
    /** @return Liczba kolejek pauzy z powodu kontuzji. */
     int getInjuryDuration() const;
    /** @return Liczba kolejek pauzy z powodu zawieszenia. */
     int getSuspensionDuration() const;

    /** @return true, jezeli zawodnik moze zagrac w najblizszym meczu. */
    bool isAvailable() const;

    /** @brief Ustawia kondycje. */
    void setCondition(double value);
    /** @brief Ustawia overall. */
    void setOverall(int value);
    /** @brief Dodaje gola. */
    void addGoal();
    /** @brief Dodaje asyste. */
    void addAssist();
    /** @brief Dodaje zolta kartke. */
    void addYellowCard();
    /** @brief Dodaje czerwona kartke. */
    void addRedCard();
    /** @brief Dodaje rozegrane minuty. */
    void addMinutesPlayed(int minutes);
    /** @brief Ustawia morale. */
    void setMorale(double value);
    /** @brief Zmienia morale o delta (z klamrowaniem do zakresu). */
    void adjustMorale(double delta);
    /** @brief Ustawia liczbe kolejek pauzy z powodu kontuzji. */
    void setInjuryDuration(int duration);
    /** @brief Ustawia liczbe kolejek pauzy z powodu zawieszenia. */
    void setSuspensionDuration(int duration);
    /** @brief Decrementuje liczbe kolejek pauzy z powodu kontuzji. */
    void decrementInjuryDuration();
    /** @brief Decrementuje liczbe kolejek pauzy z powodu zawieszenia. */
    void decrementSuspensionDuration();

    /** @return Referencja do obiektu statystyki kondycji. */
    Statistic<double>& getConditionStat() { return condition; }
    /** @return Referencja do obiektu statystyki overall. */
    Statistic<int>& getOverallStat() { return overall; }
};
