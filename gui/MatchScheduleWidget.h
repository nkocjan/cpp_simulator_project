#ifndef FIFA26_SIMULATOR_MATCHSCHEDULEWIDGET_H
#define FIFA26_SIMULATOR_MATCHSCHEDULEWIDGET_H

#include <QWidget>

#include <memory>
#include <vector>

#include "Match.h"

/**
 * @file MatchScheduleWidget.h
 * @brief Widok terminarza z podzialem na mecze rozegrane i zaplanowane.
 */

class QLabel;
class QTableWidget;

/**
 * @brief Widget prezentujacy terminarz ligi.
 */
class MatchScheduleWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Tworzy widget terminarza. */
    explicit MatchScheduleWidget(QWidget *parent = nullptr);

    /**
     * @brief Odswieza dane terminarza.
     * @param schedule Terminarz podzielony na kolejki.
     * @param currentMatchday Aktualna kolejka.
     */
    void refreshSchedule(const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                         int currentMatchday);

private:
    QLabel *titleLabel;
    QLabel *statusLabel;
    QTableWidget *playedTable;
    QTableWidget *plannedTable;

    void setupUi();
};

#endif // FIFA26_SIMULATOR_MATCHSCHEDULEWIDGET_H

