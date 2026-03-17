#ifndef FIFA26_SIMULATOR_LEAGUETABLEWIDGET_H
#define FIFA26_SIMULATOR_LEAGUETABLEWIDGET_H

#include <QWidget>
#include <QString>

#include <memory>
#include <vector>

#include "Team.h"

class QLabel;
class QPushButton;
class QTableWidget;

class LeagueTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LeagueTableWidget(QWidget *parent = nullptr);

    void refreshTable(const std::vector<std::shared_ptr<Team>> &teams,
                      int currentMatchday,
                      int totalMatchdays,
                      bool leagueFinished);

signals:
    void simulateNextRequested();
    void simulateAllRequested();
    void teamSelected(const QString &teamName);

private:
    QLabel *titleLabel;
    QLabel *statusLabel;
    QTableWidget *tableWidget;
    QPushButton *simulateNextButton;
    QPushButton *simulateAllButton;

    void setupUi();
};

#endif // FIFA26_SIMULATOR_LEAGUETABLEWIDGET_H

