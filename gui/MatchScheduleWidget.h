#ifndef FIFA26_SIMULATOR_MATCHSCHEDULEWIDGET_H
#define FIFA26_SIMULATOR_MATCHSCHEDULEWIDGET_H

#include <QWidget>

#include <memory>
#include <vector>

#include "Match.h"

class QLabel;
class QTableWidget;

class MatchScheduleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchScheduleWidget(QWidget *parent = nullptr);

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

