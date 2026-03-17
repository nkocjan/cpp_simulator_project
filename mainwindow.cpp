#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QString>

#include "core/headers/Defender.h"
#include "core/headers/Goalkeeper.h"
#include "core/headers/Midfielder.h"
#include "core/headers/Striker.h"

namespace {
const std::vector<std::string> kDefaultTeamNames = {
    "FC Krakow", "Legia Warszawa", "Lech Poznan", "Warta Poznan",
    "Pogon Szczecin", "Rakow Czestochowa", "Gornik Zabrze", "Jagiellonia Bialystok"
};

void addTestSquad(const std::shared_ptr<Team> &team, const std::string &prefix)
{
    team->addPlayer(std::make_shared<Striker>(prefix + "_ST", "One", 92.0, 84, 86));
    team->addPlayer(std::make_shared<Midfielder>(prefix + "_CM", "Two", 90.0, 81, 83));
    team->addPlayer(std::make_shared<Defender>(prefix + "_CB", "Three", 91.0, 80, 85));
    team->addPlayer(std::make_shared<Goalkeeper>(prefix + "_GK", "Four", 94.0, 82, 88));
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , leagueTabs(nullptr)
{
    ui->setupUi(this);
    setupStartPanel();
    setupLeagueViews();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStartPanel()
{
    setWindowTitle("Fifa26 Simulator - panel startowy");

    ui->leagueNameEdit->setText("Ekstraklasa Test");
    ui->teamsCountSpinBox->setValue(4);

    ui->playerTeamComboBox->clear();
    for (int i = 0; i < ui->teamsCountSpinBox->value(); ++i) {
        ui->playerTeamComboBox->addItem(QString::fromStdString(kDefaultTeamNames[static_cast<size_t>(i)]));
    }

    ui->setupStatusLabel->setText("Kliknij 'Przygotuj lige', aby utworzyc dane rozgrywek.");

    connect(ui->prepareLeagueButton, &QPushButton::clicked, this, [this]() {
        prepareLeagueData();
    });

    connect(ui->teamsCountSpinBox, &QSpinBox::valueChanged, this, [this](int value) {
        ui->playerTeamComboBox->clear();
        for (int i = 0; i < value; ++i) {
            ui->playerTeamComboBox->addItem(QString::fromStdString(kDefaultTeamNames[static_cast<size_t>(i)]));
        }
    });
}

void MainWindow::setupLeagueViews()
{
    leagueTabs = new QTabWidget(this);
    leagueTabs->setVisible(false);

    leagueTableWidget = std::make_unique<LeagueTableWidget>(leagueTabs);
    matchScheduleWidget = std::make_unique<MatchScheduleWidget>(leagueTabs);
    teamDetailsWidget = std::make_unique<TeamDetailsWidget>(leagueTabs);

    leagueTabs->addTab(leagueTableWidget.get(), "Tabela");
    leagueTabs->addTab(matchScheduleWidget.get(), "Terminarz");
    leagueTabs->addTab(teamDetailsWidget.get(), "Szczegoly druzyny");

    ui->verticalLayout->insertWidget(1, leagueTabs);

    connect(leagueTableWidget.get(), &LeagueTableWidget::simulateNextRequested, this, [this]() {
        simulateNextMatchday();
    });

    connect(leagueTableWidget.get(), &LeagueTableWidget::simulateAllRequested, this, [this]() {
        simulateWholeLeague();
    });

    connect(leagueTableWidget.get(), &LeagueTableWidget::teamSelected, this, [this](const QString &teamName) {
        if (!leagueTabs || !teamDetailsWidget) {
            return;
        }
        teamDetailsWidget->selectTeamByName(teamName);
        leagueTabs->setCurrentWidget(teamDetailsWidget.get());
    });
}

void MainWindow::prepareLeagueData()
{
    leagueEngine = std::make_unique<LeagueEngine>();

    const int teamsToCreate = ui->teamsCountSpinBox->value();
    for (int i = 0; i < teamsToCreate; ++i) {
        auto team = std::make_shared<Team>(kDefaultTeamNames[static_cast<size_t>(i)], 3000000 + (i * 500000));
        addTestSquad(team, "T" + std::to_string(i + 1));
        leagueEngine->addTeam(team);
    }

    leagueEngine->setPlayerTeam(ui->playerTeamComboBox->currentText().toStdString());
    leagueEngine->generateSchedule();

    const QString status = QString("Liga '%1' gotowa. Liczba druzyn: %2. Kolejek: %3")
                               .arg(ui->leagueNameEdit->text())
                               .arg(teamsToCreate)
                               .arg(leagueEngine->getTotalMatchdays());
    ui->setupStatusLabel->setText(status);

    leagueTabs->setVisible(true);
    refreshLeagueView();
}

void MainWindow::refreshLeagueView()
{
    if (!leagueEngine || !leagueTabs) {
        return;
    }

    leagueEngine->sortTable();

    leagueTableWidget->refreshTable(
        leagueEngine->getTeams(),
        leagueEngine->getCurrentMatchday(),
        leagueEngine->getTotalMatchdays(),
        leagueEngine->isLeagueFinished());

    matchScheduleWidget->refreshSchedule(
        leagueEngine->getSchedule(),
        leagueEngine->getCurrentMatchday());

    teamDetailsWidget->refreshData(
        leagueEngine->getTeams(),
        leagueEngine->getPlayerTeam(),
        leagueEngine->getSchedule());
}

void MainWindow::simulateNextMatchday()
{
    if (!leagueEngine || leagueEngine->isLeagueFinished()) {
        return;
    }

    leagueEngine->simulateNextMatchday();
    refreshLeagueView();
}

void MainWindow::simulateWholeLeague()
{
    if (!leagueEngine) {
        return;
    }

    while (!leagueEngine->isLeagueFinished()) {
        leagueEngine->simulateNextMatchday();
    }

    refreshLeagueView();
}
