#pragma once

#include <algorithm>
#include <string>
#include <utility>

/**
 * @file Statistic.h
 * @brief Szablon generycznej statystyki z ograniczeniami min/max.
 */

/**
 * @brief Pojedyncza statystyka liczbowa pilkarza, druzyny lub meczu.
 * @tparam T Typ liczbowy przechowywanej wartosci.
 */
template <typename T>
class Statistic {
private:
    std::string name;
    T value;
    T minValue;
    T maxValue;

public:
    /**
     * @brief Tworzy statystyke.
     * @param statName Nazwa statystyki.
     * @param initValue Wartosc poczatkowa.
     * @param minVal Dolna granica.
     * @param maxVal Gorna granica.
     */
    Statistic(std::string statName, T initValue, T minVal, T maxVal)
        : name(std::move(statName)), value(initValue), minValue(minVal), maxValue(maxVal) {
        value = std::clamp(value, minValue, maxValue);
    }

    /** @brief Dodaje wartosc i klamruje wynik do zakresu min/max. */
    void add(T amount) {
        value = std::clamp(static_cast<T>(value + amount), minValue, maxValue);
    }

    /** @brief Odejmuje wartosc i klamruje wynik do zakresu min/max. */
    void subtract(T amount) {
        value = std::clamp(static_cast<T>(value - amount), minValue, maxValue);
    }

    /** @brief Ustawia nowa wartosc w granicach min/max. */
    void setValue(T newValue) {
        value = std::clamp(newValue, minValue, maxValue);
    }

    /** @return Aktualna wartosc statystyki. */
    [[nodiscard]] T getValue() const {
        return value;
    }

    /** @return Nazwa statystyki. */
    [[nodiscard]] const std::string& getName() const {
        return name;
    }
};
