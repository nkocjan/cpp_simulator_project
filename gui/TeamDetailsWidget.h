#ifndef FIFA26_SIMULATOR_TEAMDETAILSWIDGET_H
#define FIFA26_SIMULATOR_TEAMDETAILSWIDGET_H

#include <QWidget>

#include <memory>
#include <vector>

#include "Match.h"
#include "Team.h"

class QComboBox;
class QLabel;
class QListWidget;
class QTableWidget;

class TeamDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TeamDetailsWidget(QWidget *parent = nullptr);

    void refreshData(const std::vector<std::shared_ptr<Team>> &teams,
                     const std::shared_ptr<Team> &preferredTeam,
                     const std::vector<std::vector<std::shared_ptr<Match>>> &schedule);
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

