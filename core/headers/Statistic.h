#pragma once

#include <algorithm>
#include <string>
#include <utility>

template <typename T>
class Statistic {
private:
    std::string name;
    T value;
    T minValue;
    T maxValue;

public:
    Statistic(std::string statName, T initValue, T minVal, T maxVal)
        : name(std::move(statName)), value(initValue), minValue(minVal), maxValue(maxVal) {
        value = std::clamp(value, minValue, maxValue);
    }

    void add(T amount) {
        value = std::clamp(static_cast<T>(value + amount), minValue, maxValue);
    }

    void subtract(T amount) {
        value = std::clamp(static_cast<T>(value - amount), minValue, maxValue);
    }

    void setValue(T newValue) {
        value = std::clamp(newValue, minValue, maxValue);
    }

    [[nodiscard]] T getValue() const {
        return value;
    }

    [[nodiscard]] const std::string& getName() const {
        return name;
    }
};

