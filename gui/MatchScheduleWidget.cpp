#include "MatchScheduleWidget.h"

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

MatchScheduleWidget::MatchScheduleWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , statusLabel(nullptr)
    , playedTable(nullptr)
    , plannedTable(nullptr)
{
    setupUi();
}

void MatchScheduleWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);

    titleLabel = new QLabel("Terminarz - rozegrane i zaplanowane spotkania", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    statusLabel = new QLabel("Brak danych terminarza.", this);

    auto configureTable = [](QTableWidget *table) {
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"Kolejka", "Gospodarz", "Gosc", "Wynik", "Skrot"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
    };

    auto *playedTitle = new QLabel("Rozegrane spotkania", this);
    playedTable = new QTableWidget(this);
    configureTable(playedTable);

    auto *plannedTitle = new QLabel("Zaplanowane spotkania", this);
    plannedTable = new QTableWidget(this);
    configureTable(plannedTable);

    layout->addWidget(titleLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(playedTitle);
    layout->addWidget(playedTable);
    layout->addWidget(plannedTitle);
    layout->addWidget(plannedTable);
}

void MatchScheduleWidget::refreshSchedule(const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                                          int currentMatchday)
{
    int playedCount = 0;
    int plannedCount = 0;
    for (const auto &matchday : schedule) {
        for (const auto &match : matchday) {
            if (match->getIsFinished()) {
                ++playedCount;
            } else {
                ++plannedCount;
            }
        }
    }

    playedTable->setRowCount(playedCount);
    plannedTable->setRowCount(plannedCount);

    int playedRow = 0;
    int plannedRow = 0;

    for (int day = 0; day < static_cast<int>(schedule.size()); ++day) {
        int matchInDay = 0;
        for (const auto &match : schedule[static_cast<size_t>(day)]) {
            ++matchInDay;
            const QString home = QString::fromStdString(match->getHomeTeam()->getName());
            const QString away = QString::fromStdString(match->getAwayTeam()->getName());
            const QString code = QString("MD%1-M%2").arg(day + 1).arg(matchInDay);

            if (match->getIsFinished()) {
                playedTable->setItem(playedRow, 0, new QTableWidgetItem(QString::number(day + 1)));
                playedTable->setItem(playedRow, 1, new QTableWidgetItem(home));
                playedTable->setItem(playedRow, 2, new QTableWidgetItem(away));
                playedTable->setItem(playedRow, 3,
                                     new QTableWidgetItem(QString("%1 : %2").arg(match->getHomeGoals()).arg(match->getAwayGoals())));
                playedTable->setItem(playedRow, 4, new QTableWidgetItem(code));
                ++playedRow;
            } else {
                plannedTable->setItem(plannedRow, 0, new QTableWidgetItem(QString::number(day + 1)));
                plannedTable->setItem(plannedRow, 1, new QTableWidgetItem(home));
                plannedTable->setItem(plannedRow, 2, new QTableWidgetItem(away));
                plannedTable->setItem(plannedRow, 3, new QTableWidgetItem("-"));
                plannedTable->setItem(plannedRow, 4, new QTableWidgetItem(code));
                ++plannedRow;
            }
        }
    }

    statusLabel->setText(QString("Aktualna kolejka: %1 / %2 | Rozegrane: %3 | Zaplanowane: %4")
                             .arg(currentMatchday)
                             .arg(schedule.size())
                             .arg(playedCount)
                             .arg(plannedCount));
}
