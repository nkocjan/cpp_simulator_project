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
class QLineEdit;
class QPushButton;
class QTableWidget;
class QListWidget;

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

    /**
     * @brief Wlacza lub blokuje edycje terminarza.
     * @param enabled true - edycja aktywna, false - edycja zablokowana.
     */
    void setEditingEnabled(bool enabled);

signals:
    /** @brief Prosba o zmiane slotu meczu (tylko nierozegrane). */
    void matchSlotEditRequested(int matchdayIndex, int matchIndex, const QString& newSlot);
    /** @brief Sygnał do przejścia do następnego etapu przygotowania. */
    void phaseAdvanceRequested();

private:
    QLabel *titleLabel;
    QLabel *statusLabel;
    QTableWidget *playedTable;
    QTableWidget *plannedTable;
    QLineEdit *slotEdit;
    QPushButton *applySlotButton;
    QPushButton *confirmScheduleButton;
    QLabel *matchDetailsLabel;
    QListWidget *matchDetailsList;

    int selectedMatchdayIndex;
    int selectedMatchIndex;
    bool editingEnabled;
    std::vector<std::vector<std::shared_ptr<Match>>> scheduleCache;

    void setupUi();
    void showMatchDetails(int matchdayIndex, int matchIndex);
};

#endif
