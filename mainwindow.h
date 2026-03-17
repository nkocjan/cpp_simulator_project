#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "core/headers/LeagueEngine.h"
#include "gui/LeagueTableWidget.h"
#include "gui/MatchScheduleWidget.h"
#include "gui/TeamDetailsWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<LeagueEngine> leagueEngine;
    std::unique_ptr<LeagueTableWidget> leagueTableWidget;
    std::unique_ptr<MatchScheduleWidget> matchScheduleWidget;
    std::unique_ptr<TeamDetailsWidget> teamDetailsWidget;
    class QTabWidget *leagueTabs;

    void setupStartPanel();
    void prepareLeagueData();
    void setupLeagueViews();
    void refreshLeagueView();
    void simulateNextMatchday();
    void simulateWholeLeague();
};

#endif // MAINWINDOW_H
