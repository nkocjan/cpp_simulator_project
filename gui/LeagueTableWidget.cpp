#include "LeagueTableWidget.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

LeagueTableWidget::LeagueTableWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , statusLabel(nullptr)
    , tableWidget(nullptr)
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

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(tableWidget);
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

void LeagueTableWidget::refreshTable(const std::vector<std::shared_ptr<Team>> &teams,
                                     int currentMatchday,
                                     int totalMatchdays,
                                     bool leagueFinished)
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
}

