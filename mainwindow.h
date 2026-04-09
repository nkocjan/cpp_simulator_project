#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <string>
#include <vector>

#include "core/headers/LeagueEngine.h"
#include "gui/LeagueTableWidget.h"
#include "gui/MatchScheduleWidget.h"
#include "gui/TeamDetailsWidget.h"
#include "gui/PlayerDetailsWidget.h"

/**
 * @file mainwindow.h
 * @brief Glowne okno aplikacji Qt i orchestrator widokow ligi.
 */

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Glowny kontroler warstwy GUI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** @brief Tworzy glowne okno aplikacji. */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum class SetupPhase {
        Configuration,
        Transfers,
        Schedule,
        League
    };

    Ui::MainWindow *ui;
    std::unique_ptr<LeagueEngine> leagueEngine;
    std::unique_ptr<LeagueTableWidget> leagueTableWidget;
    std::unique_ptr<MatchScheduleWidget> matchScheduleWidget;
    std::unique_ptr<TeamDetailsWidget> teamDetailsWidget;
    std::unique_ptr<PlayerDetailsWidget> playerDetailsWidget;
    class QTabWidget *leagueTabs;
    std::vector<std::string> availableTeamPool;
    SetupPhase setupPhase;

    /** @brief Konfiguruje panel startowy. */
    void setupStartPanel();
    /** @brief Tworzy dane ligi na podstawie ustawien startowych. */
    void prepareLeagueData();
    /** @brief Inicjalizuje widoki zakladek ligowych. */
    void setupLeagueViews();
    /** @brief Odswieza wszystkie widoki ligi. */
    void refreshLeagueView();
    /** @brief Symuluje nastepna kolejke i odswieza GUI. */
    void simulateNextMatchday();
    /** @brief Symuluje cala lige do konca i odswieza GUI. */
    void simulateWholeLeague();
    void refreshTeamSelectionControls();
    std::vector<std::string> getCheckedTeams() const;
    void returnToSetupView();
    void advanceToTransfersPhase();
    void advanceToSchedulePhase();
    void advanceToLeaguePhase();
};

#endif
