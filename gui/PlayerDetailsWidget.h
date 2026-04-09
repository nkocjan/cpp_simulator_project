#ifndef FIFA26_SIMULATOR_PLAYERDETAILSWIDGET_H
#define FIFA26_SIMULATOR_PLAYERDETAILSWIDGET_H

#include <QWidget>
#include <memory>
#include <vector>

#include "Player.h"
#include "Team.h"

/**
 * @file PlayerDetailsWidget.h
 * @brief Widok szczegółów i statystyk wybranego zawodnika.
 */

class QLabel;
class QTableWidget;
class QComboBox;

/**
 * @brief Widget prezentujący szczegółowe informacje o zawodniku.
 *
 * Wyświetla:
 * - Dane osobowe i pozycja
 * - Statystyki sezonowe (gole, asysty, kartki, minuty)
 * - Historia kontuzji i zawieszenia
 * - Trend warości statystyk
 */
class PlayerDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Tworzy widget szczegółów zawodnika. */
    explicit PlayerDetailsWidget(QWidget *parent = nullptr);

    /**
     * @brief Odswieza liste zawodnikow do wyboru w zakladce.
     * @param teams Druzyny ligi.
     * @param preferredTeam Preferowana druzyna (np. druzyna gracza).
     */
    void refreshPlayers(const std::vector<std::shared_ptr<Team>>& teams,
                        const std::shared_ptr<Team>& preferredTeam);

    /**
     * @brief Wyświetla szczegóły wybranego zawodnika.
     * @param player Wskaźnik do zawodnika do wyświetlenia.
     * @param team Drużyna, do której należy zawodnik.
     */
    void displayPlayerDetails(const std::shared_ptr<Player>& player,
                              const std::shared_ptr<Team>& team);

private:
    QComboBox *playerSelector;
    QLabel *nameLabel;
    QLabel *positionLabel;
    QLabel *overallLabel;
    QLabel *conditionLabel;
    QLabel *statusLabel;

    QLabel *goalsLabel;
    QLabel *assistsLabel;
    QLabel *yellowCardsLabel;
    QLabel *redCardsLabel;
    QLabel *minutesPlayedLabel;

    QLabel *injuryLabel;
    QLabel *suspensionLabel;
    QLabel *moraleLabel;

    QTableWidget *statsTable;
    QTableWidget *matchHistoryTable;

    std::vector<std::shared_ptr<Team>> teamsCache;

    void setupUi();
    void updateStats(const std::shared_ptr<Player>& player);
};

#endif
