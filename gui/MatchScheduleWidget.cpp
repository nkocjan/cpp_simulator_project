#include "MatchScheduleWidget.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

MatchScheduleWidget::MatchScheduleWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , statusLabel(nullptr)
    , playedTable(nullptr)
    , plannedTable(nullptr)
    , slotEdit(nullptr)
    , applySlotButton(nullptr)
    , confirmScheduleButton(nullptr)
    , matchDetailsLabel(nullptr)
    , matchDetailsList(nullptr)
    , selectedMatchdayIndex(-1)
    , selectedMatchIndex(-1)
    , editingEnabled(true)
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
        table->setColumnCount(6);
        table->setHorizontalHeaderLabels({"Kolejka", "Gospodarz", "Gosc", "Wynik", "Termin", "Skrot"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
    };

    auto *playedTitle = new QLabel("Rozegrane spotkania", this);
    playedTable = new QTableWidget(this);
    configureTable(playedTable);

    auto *plannedTitle = new QLabel("Zaplanowane spotkania", this);
    plannedTable = new QTableWidget(this);
    configureTable(plannedTable);

    auto *editLayout = new QHBoxLayout();
    slotEdit = new QLineEdit(this);
    slotEdit->setPlaceholderText("Nowy termin, np. Sobota 17:30");
    applySlotButton = new QPushButton("Zmien termin meczu", this);
    editLayout->addWidget(slotEdit);
    editLayout->addWidget(applySlotButton);

    auto *confirmLayout = new QHBoxLayout();
    confirmScheduleButton = new QPushButton("Zatwierdz terminarz -> Liga", this);
    confirmLayout->addStretch();
    confirmLayout->addWidget(confirmScheduleButton);

    matchDetailsLabel = new QLabel("Szczegoly spotkania: wybierz mecz z tabeli.", this);
    matchDetailsList = new QListWidget(this);
    matchDetailsList->setMinimumHeight(180);

    layout->addWidget(titleLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(playedTitle);
    layout->addWidget(playedTable);
    layout->addWidget(plannedTitle);
    layout->addWidget(plannedTable);
    layout->addLayout(editLayout);
    layout->addLayout(confirmLayout);
    layout->addWidget(matchDetailsLabel);
    layout->addWidget(matchDetailsList);

    connect(confirmScheduleButton, &QPushButton::clicked, this, [this]() {
        emit phaseAdvanceRequested();
    });

    auto handleTableClick = [this](QTableWidget *table, int row) {
        auto *item = table ? table->item(row, 0) : nullptr;
        if (!item) {
            return;
        }

        const QString key = item->data(Qt::UserRole).toString();
        const QStringList parts = key.split(':');
        if (parts.size() != 2) {
            return;
        }

        const int dayIdx = parts[0].toInt();
        const int matchIdx = parts[1].toInt();
        showMatchDetails(dayIdx, matchIdx);

        if (table == plannedTable) {
            selectedMatchdayIndex = dayIdx;
            selectedMatchIndex = matchIdx;
            slotEdit->setText(table->item(row, 4) ? table->item(row, 4)->text() : QString());
        }
    };

    connect(playedTable, &QTableWidget::cellClicked, this, [handleTableClick, this](int row, int) {
        handleTableClick(playedTable, row);
    });

    connect(plannedTable, &QTableWidget::cellClicked, this, [handleTableClick, this](int row, int) {
        handleTableClick(plannedTable, row);
    });

    connect(applySlotButton, &QPushButton::clicked, this, [this]() {
        if (selectedMatchdayIndex < 0 || selectedMatchIndex < 0) {
            return;
        }
        if (slotEdit->text().trimmed().isEmpty()) {
            return;
        }

        emit matchSlotEditRequested(selectedMatchdayIndex, selectedMatchIndex, slotEdit->text().trimmed());
    });
}

void MatchScheduleWidget::setEditingEnabled(bool enabled)
{
    editingEnabled = enabled;
    slotEdit->setEnabled(enabled);
    applySlotButton->setEnabled(enabled);
    confirmScheduleButton->setEnabled(enabled);

    if (!enabled) {
        selectedMatchdayIndex = -1;
        selectedMatchIndex = -1;
    }
}

void MatchScheduleWidget::showMatchDetails(int matchdayIndex, int matchIndex)
{
    matchDetailsList->clear();

    if (matchdayIndex < 0 || matchdayIndex >= static_cast<int>(scheduleCache.size())) {
        matchDetailsLabel->setText("Szczegoly spotkania: brak danych.");
        return;
    }

    const auto& dayMatches = scheduleCache[static_cast<size_t>(matchdayIndex)];
    if (matchIndex < 0 || matchIndex >= static_cast<int>(dayMatches.size())) {
        matchDetailsLabel->setText("Szczegoly spotkania: brak danych.");
        return;
    }

    const auto& match = dayMatches[static_cast<size_t>(matchIndex)];
    const QString title = QString("Szczegoly spotkania: K%1 | %2 vs %3 | %4")
                              .arg(matchdayIndex + 1)
                              .arg(QString::fromStdString(match->getHomeTeam()->getName()))
                              .arg(QString::fromStdString(match->getAwayTeam()->getName()))
                              .arg(QString::fromStdString(match->getWeekendSlot()));
    matchDetailsLabel->setText(title);

    if (!match->getIsFinished()) {
        matchDetailsList->addItem("Mecz jeszcze nie zostal rozegrany.");
        return;
    }

    matchDetailsList->addItem(QString("Wynik: %1 : %2")
                                  .arg(match->getHomeGoals())
                                  .arg(match->getAwayGoals()));

    const auto& events = match->getMatchEvents();
    if (events.empty()) {
        matchDetailsList->addItem("Brak zarejestrowanych zdarzen meczu.");
        return;
    }

    for (const auto& eventLine : events) {
        matchDetailsList->addItem(QString::fromStdString(eventLine));
    }
}

void MatchScheduleWidget::refreshSchedule(const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                                          int currentMatchday)
{
    scheduleCache = schedule;

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
            const int localMatchIndex = matchInDay;
            ++matchInDay;

            const QString home = QString::fromStdString(match->getHomeTeam()->getName());
            const QString away = QString::fromStdString(match->getAwayTeam()->getName());
            const QString code = QString("MD%1-M%2").arg(day + 1).arg(matchInDay);
            const QString slot = QString::fromStdString(match->getWeekendSlot());

            if (match->getIsFinished()) {
                auto *cell = new QTableWidgetItem(QString::number(day + 1));
                cell->setData(Qt::UserRole, QString("%1:%2").arg(day).arg(localMatchIndex));
                playedTable->setItem(playedRow, 0, cell);
                playedTable->setItem(playedRow, 1, new QTableWidgetItem(home));
                playedTable->setItem(playedRow, 2, new QTableWidgetItem(away));
                playedTable->setItem(playedRow, 3,
                                     new QTableWidgetItem(QString("%1 : %2").arg(match->getHomeGoals()).arg(match->getAwayGoals())));
                playedTable->setItem(playedRow, 4, new QTableWidgetItem(slot));
                playedTable->setItem(playedRow, 5, new QTableWidgetItem(code));
                ++playedRow;
            } else {
                auto *cell = new QTableWidgetItem(QString::number(day + 1));
                cell->setData(Qt::UserRole, QString("%1:%2").arg(day).arg(localMatchIndex));
                plannedTable->setItem(plannedRow, 0, cell);
                plannedTable->setItem(plannedRow, 1, new QTableWidgetItem(home));
                plannedTable->setItem(plannedRow, 2, new QTableWidgetItem(away));
                plannedTable->setItem(plannedRow, 3, new QTableWidgetItem("-"));
                plannedTable->setItem(plannedRow, 4, new QTableWidgetItem(slot));
                plannedTable->setItem(plannedRow, 5, new QTableWidgetItem(code));
                ++plannedRow;
            }
        }
    }

    statusLabel->setText(QString("Aktualna kolejka: %1 / %2 | Rozegrane: %3 | Zaplanowane: %4")
                             .arg(currentMatchday)
                             .arg(schedule.size())
                             .arg(playedCount)
                             .arg(plannedCount));

    if (selectedMatchdayIndex >= 0 && selectedMatchIndex >= 0) {
        showMatchDetails(selectedMatchdayIndex, selectedMatchIndex);
    } else {
        matchDetailsLabel->setText("Szczegoly spotkania: wybierz mecz z tabeli.");
        matchDetailsList->clear();
    }

    setEditingEnabled(editingEnabled);
}