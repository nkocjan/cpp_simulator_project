#include "TeamDetailsWidget.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TeamDetailsWidget::TeamDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , teamSelector(nullptr)
    , teamStatsLabel(nullptr)
    , playersTable(nullptr)
    , historyList(nullptr)
    , upcomingList(nullptr)
{
    setupUi();
}

void TeamDetailsWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Szczegoly druzyny", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    title->setFont(titleFont);

    teamSelector = new QComboBox(this);
    teamStatsLabel = new QLabel("Wybierz druzyne.", this);

    playersTable = new QTableWidget(this);
    playersTable->setColumnCount(8);
    playersTable->setHorizontalHeaderLabels({"Imie", "Nazwisko", "OVR", "Kond.", "G", "A", "Kartki", "Min"});
    playersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    playersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playersTable->setSelectionMode(QAbstractItemView::NoSelection);

    auto *historyTitle = new QLabel("Historia meczow:", this);
    historyList = new QListWidget(this);

    auto *upcomingTitle = new QLabel("Nastepne spotkania:", this);
    upcomingList = new QListWidget(this);

    layout->addWidget(title);
    layout->addWidget(teamSelector);
    layout->addWidget(teamStatsLabel);
    layout->addWidget(playersTable);
    layout->addWidget(historyTitle);
    layout->addWidget(historyList);
    layout->addWidget(upcomingTitle);
    layout->addWidget(upcomingList);

    connect(teamSelector, &QComboBox::currentIndexChanged, this, [this]() {
        renderSelectedTeam();
    });
}

void TeamDetailsWidget::refreshData(const std::vector<std::shared_ptr<Team>> &teams,
                                    const std::shared_ptr<Team> &preferredTeam,
                                    const std::vector<std::vector<std::shared_ptr<Match>>> &schedule)
{
    teamsCache = teams;
    scheduleCache = schedule;

    const QString oldSelection = teamSelector->currentText();

    teamSelector->blockSignals(true);
    teamSelector->clear();
    for (const auto &team : teamsCache) {
        teamSelector->addItem(QString::fromStdString(team->getName()));
    }

    int index = teamSelector->findText(oldSelection);
    if (index < 0 && preferredTeam) {
        index = teamSelector->findText(QString::fromStdString(preferredTeam->getName()));
    }
    if (index < 0 && !teamsCache.empty()) {
        index = 0;
    }

    teamSelector->setCurrentIndex(index);
    teamSelector->blockSignals(false);

    renderSelectedTeam();
}

void TeamDetailsWidget::selectTeamByName(const QString &teamName)
{
    const int index = teamSelector->findText(teamName);
    if (index < 0) {
        return;
    }

    teamSelector->setCurrentIndex(index);
    renderSelectedTeam();
}

void TeamDetailsWidget::renderSelectedTeam()
{
    const int index = teamSelector->currentIndex();
    if (index < 0 || index >= static_cast<int>(teamsCache.size())) {
        teamStatsLabel->setText("Brak danych druzyny.");
        playersTable->setRowCount(0);
        historyList->clear();
        upcomingList->clear();
        return;
    }

    const auto &team = teamsCache[static_cast<size_t>(index)];

    const int diff = team->getGoalsScored() - team->getGoalsConceded();
    teamStatsLabel->setText(
        QString("%1 | Pkt: %2 | M: %3 | W: %4 R: %5 P: %6 | Bramki: %7-%8 | Bilans: %9")
            .arg(QString::fromStdString(team->getName()))
            .arg(team->getPoints())
            .arg(team->getMatchesPlayed())
            .arg(team->getWins())
            .arg(team->getDraws())
            .arg(team->getLosses())
            .arg(team->getGoalsScored())
            .arg(team->getGoalsConceded())
            .arg(diff));

    const auto &players = team->getPlayers();
    playersTable->setRowCount(static_cast<int>(players.size()));

    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        const auto &player = players[static_cast<size_t>(i)];
        const QString cards = QString("%1/%2").arg(player->getYellowCards()).arg(player->getRedCards());

        playersTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(player->getName())));
        playersTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(player->getSurname())));
        playersTable->setItem(i, 2, new QTableWidgetItem(QString::number(player->getOverall())));
        playersTable->setItem(i, 3, new QTableWidgetItem(QString::number(player->getCondition(), 'f', 1)));
        playersTable->setItem(i, 4, new QTableWidgetItem(QString::number(player->getGoals())));
        playersTable->setItem(i, 5, new QTableWidgetItem(QString::number(player->getAssists())));
        playersTable->setItem(i, 6, new QTableWidgetItem(cards));
        playersTable->setItem(i, 7, new QTableWidgetItem(QString::number(player->getMinutesPlayed())));
    }

    historyList->clear();
    upcomingList->clear();

    for (int day = 0; day < static_cast<int>(scheduleCache.size()); ++day) {
        for (const auto &match : scheduleCache[static_cast<size_t>(day)]) {
            const bool isTeamMatch = (match->getHomeTeam() == team) || (match->getAwayTeam() == team);
            if (!isTeamMatch) {
                continue;
            }

            const QString line = QString("K%1: %2 vs %3")
                                     .arg(day + 1)
                                     .arg(QString::fromStdString(match->getHomeTeam()->getName()))
                                     .arg(QString::fromStdString(match->getAwayTeam()->getName()));

            if (match->getIsFinished()) {
                historyList->addItem(line + QString(" | %1:%2").arg(match->getHomeGoals()).arg(match->getAwayGoals()));
            } else {
                upcomingList->addItem(line);
            }
        }
    }
}

