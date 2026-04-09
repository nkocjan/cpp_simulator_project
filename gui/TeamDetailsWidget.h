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
class QPushButton;
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

    /**
     * @brief Globalnie wlacza lub blokuje sekcje transferow.
     * @param enabled true - edycja aktywna, false - edycja zablokowana.
     */
    void setTransferEditorEnabled(bool enabled);

signals:
    /** @brief Emitowany po ręcznej zmianie składu wyjściowego. */
    void lineupEdited();
    /** @brief Prosba o pokazanie szczegolow wybranego zawodnika. */
    void playerSelectedRequested(const QString& teamName, int playerIndex);
    /** @brief Prosba o transfer zawodnika miedzy klubami. */
    void transferRequested(const QString& fromTeam,
                           const QString& toTeam,
                           const QString& playerName,
                           const QString& playerSurname);
    /** @brief Sygnał do przejścia do następnego etapu przygotowania. */
    void phaseAdvanceRequested();

private:
    QComboBox *teamSelector;
    QLabel *teamStatsLabel;
    QTableWidget *playersTable;
    QListWidget *historyList;
    QListWidget *upcomingList;
    QListWidget *unavailableList;

    QComboBox *gkStarterCombo;
    QComboBox *defStarterCombo;
    QComboBox *midStarterCombo;
    QComboBox *stStarterCombo;
    QPushButton *applyLineupButton;
    QLabel *lineupEditorInfoLabel;

    QComboBox *transferFromTeamCombo;
    QComboBox *transferPlayerCombo;
    QPushButton *transferButton;
    QPushButton *confirmTransfersButton;
    QLabel *transferInfoLabel;

    std::vector<std::shared_ptr<Team>> teamsCache;
    std::vector<std::vector<std::shared_ptr<Match>>> scheduleCache;
    std::shared_ptr<Team> playerTeamCache;
    bool transferEditorEnabled;

    void setupUi();
    void renderSelectedTeam();
    void populateLineupEditor(const std::shared_ptr<Team>& team, bool isPlayerTeam);
    std::shared_ptr<Player> comboSelectionToPlayer(QComboBox *combo, const std::shared_ptr<Team>& team) const;
    void populateTransferEditor(const std::shared_ptr<Team>& selectedTeam, bool isPlayerTeam);
};

#endif
