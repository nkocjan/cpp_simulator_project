#ifndef FIFA26_SIMULATOR_TEAMDETAILSWIDGET_H
#define FIFA26_SIMULATOR_TEAMDETAILSWIDGET_H

#include <QWidget>

#include <memory>
#include <vector>

#include "Match.h"
#include "Team.h"

/**
 * @file TeamDetailsWidget.h
 * @brief Widok szczegolow wybranej druzyny.
 */

class QComboBox;
class QLabel;
class QListWidget;
class QTableWidget;

/**
 * @brief Widget prezentujacy sklad i statystyki druzyny.
 */
class TeamDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Tworzy widget szczegolow druzyny. */
    explicit TeamDetailsWidget(QWidget *parent = nullptr);

    /**
     * @brief Odswieza caly widok na podstawie danych ligi.
     * @param teams Lista druzyn.
     * @param preferredTeam Domyslnie wskazana druzyna.
     * @param schedule Terminarz meczow.
     */
    void refreshData(const std::vector<std::shared_ptr<Team>> &teams,
                     const std::shared_ptr<Team> &preferredTeam,
                     const std::vector<std::vector<std::shared_ptr<Match>>> &schedule);

    /**
     * @brief Ustawia aktywna druzyne po nazwie.
     * @param teamName Nazwa druzyny do zaznaczenia.
     */
    void selectTeamByName(const QString &teamName);

private:
    QComboBox *teamSelector;
    QLabel *teamStatsLabel;
    QTableWidget *playersTable;
    QListWidget *historyList;
    QListWidget *upcomingList;

    std::vector<std::shared_ptr<Team>> teamsCache;
    std::vector<std::vector<std::shared_ptr<Match>>> scheduleCache;

    void setupUi();
    void renderSelectedTeam();
};

#endif // FIFA26_SIMULATOR_TEAMDETAILSWIDGET_H

