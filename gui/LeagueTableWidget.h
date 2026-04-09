#ifndef FIFA26_SIMULATOR_LEAGUETABLEWIDGET_H
#define FIFA26_SIMULATOR_LEAGUETABLEWIDGET_H

#include <QWidget>
#include <QString>

#include <memory>
#include <vector>

#include "Match.h"
#include "Team.h"

/**
 * @file LeagueTableWidget.h
 * @brief Widok tabeli ligowej z akcjami symulacji.
 */

class QLabel;
class QPushButton;
class QTableWidget;
class QListWidget;

/**
 * @brief Widget prezentujacy tabele ligi.
 */
class LeagueTableWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Tworzy widget tabeli. */
    explicit LeagueTableWidget(QWidget *parent = nullptr);

    /**
     * @brief Odswieza dane tabeli.
     * @param teams Druzyny posortowane wedlug tabeli.
     * @param currentMatchday Aktualna kolejka.
     * @param totalMatchdays Wszystkie kolejki.
     * @param leagueFinished Flaga zakonczenia ligi.
     */
    void refreshTable(const std::vector<std::shared_ptr<Team>> &teams,
                      int currentMatchday,
                      int totalMatchdays,
                      bool leagueFinished,
                      const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                      const std::shared_ptr<Team> &playerTeam);

signals:
    /** @brief Zgloszenie prosby o symulacje kolejnej kolejki. */
    void simulateNextRequested();
    /** @brief Zgloszenie prosby o symulacje calej ligi. */
    void simulateAllRequested();
    /** @brief Emisja po wybraniu nazwy druzyny w tabeli. */
    void teamSelected(const QString &teamName);

private:
    QLabel *titleLabel;
    QLabel *statusLabel;
    QTableWidget *tableWidget;
    QListWidget *playerTeamLogList;
    QPushButton *simulateNextButton;
    QPushButton *simulateAllButton;

    void setupUi();
    void refreshPlayerTeamLog(const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                              const std::shared_ptr<Team> &playerTeam);
};

#endif
