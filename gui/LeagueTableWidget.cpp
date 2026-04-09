#include "LeagueTableWidget.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

LeagueTableWidget::LeagueTableWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , statusLabel(nullptr)
    , tableWidget(nullptr)
    , playerTeamLogList(nullptr)
    , simulateNextButton(nullptr)
    , simulateAllButton(nullptr)
{
    setupUi();
}

void LeagueTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("Aktualna tabela ligowa", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    statusLabel = new QLabel("Liga nieprzygotowana.", this);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(10);
    tableWidget->setHorizontalHeaderLabels(
        {"Poz", "Druzyna", "Pkt", "M", "W", "R", "P", "+", "-", "Bilans"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    auto *buttonsLayout = new QHBoxLayout();
    simulateNextButton = new QPushButton("Symuluj kolejna kolejke", this);
    simulateAllButton = new QPushButton("Symuluj cala lige", this);

    buttonsLayout->addWidget(simulateNextButton);
    buttonsLayout->addWidget(simulateAllButton);

    auto *logTitleLabel = new QLabel("Log akcji Twojej druzyny", this);
    QFont logFont = logTitleLabel->font();
    logFont.setBold(true);
    logTitleLabel->setFont(logFont);

    playerTeamLogList = new QListWidget(this);
    playerTeamLogList->setMinimumHeight(170);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(tableWidget);
    mainLayout->addWidget(logTitleLabel);
    mainLayout->addWidget(playerTeamLogList);
    mainLayout->addLayout(buttonsLayout);

    connect(simulateNextButton, &QPushButton::clicked, this, &LeagueTableWidget::simulateNextRequested);
    connect(simulateAllButton, &QPushButton::clicked, this, &LeagueTableWidget::simulateAllRequested);
    connect(tableWidget, &QTableWidget::cellClicked, this, [this](int row, int column) {
        if (column != 1) {
            return;
        }
        QTableWidgetItem *item = tableWidget->item(row, column);
        if (item) {
            emit teamSelected(item->text());
        }
    });
}

void LeagueTableWidget::refreshPlayerTeamLog(const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                                             const std::shared_ptr<Team> &playerTeam)
{
    playerTeamLogList->clear();

    if (!playerTeam) {
        playerTeamLogList->addItem("Najpierw wybierz druzyne gracza.");
        return;
    }

    bool anyEvent = false;
    for (int day = 0; day < static_cast<int>(schedule.size()); ++day) {
        for (const auto& match : schedule[static_cast<size_t>(day)]) {
            const bool isPlayerTeamMatch =
                (match->getHomeTeam() == playerTeam) || (match->getAwayTeam() == playerTeam);
            if (!isPlayerTeamMatch || !match->getIsFinished()) {
                continue;
            }

            anyEvent = true;
            const QString header = QString("Kolejka %1 | %2 vs %3 | wynik %4:%5")
                                       .arg(day + 1)
                                       .arg(QString::fromStdString(match->getHomeTeam()->getName()))
                                       .arg(QString::fromStdString(match->getAwayTeam()->getName()))
                                       .arg(match->getHomeGoals())
                                       .arg(match->getAwayGoals());
            playerTeamLogList->addItem(header);

            const auto& events = match->getMatchEvents();
            for (const auto& eventLine : events) {
                playerTeamLogList->addItem("  - " + QString::fromStdString(eventLine));
            }
        }
    }

    if (!anyEvent) {
        playerTeamLogList->addItem("Brak rozegranych meczow Twojej druzyny.");
    }
}

void LeagueTableWidget::refreshTable(const std::vector<std::shared_ptr<Team>> &teams,
                                     int currentMatchday,
                                     int totalMatchdays,
                                     bool leagueFinished,
                                     const std::vector<std::vector<std::shared_ptr<Match>>> &schedule,
                                     const std::shared_ptr<Team> &playerTeam)
{
    tableWidget->setRowCount(static_cast<int>(teams.size()));

    for (int i = 0; i < static_cast<int>(teams.size()); ++i) {
        const auto &team = teams[static_cast<size_t>(i)];
        const int goalDiff = team->getGoalsScored() - team->getGoalsConceded();

        tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(team->getName())));
        tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(team->getPoints())));
        tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(team->getMatchesPlayed())));
        tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(team->getWins())));
        tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(team->getDraws())));
        tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(team->getLosses())));
        tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(team->getGoalsScored())));
        tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(team->getGoalsConceded())));
        tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(goalDiff)));
    }

    statusLabel->setText(QString("Kolejka: %1 / %2")
                             .arg(currentMatchday)
                             .arg(totalMatchdays));

    simulateNextButton->setEnabled(!leagueFinished);
    simulateAllButton->setEnabled(!leagueFinished);

    if (leagueFinished) {
        statusLabel->setText(statusLabel->text() + " (Liga zakonczona)");
    }

    refreshPlayerTeamLog(schedule, playerTeam);
}
