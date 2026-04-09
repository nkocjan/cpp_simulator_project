#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QString>
#include <QVBoxLayout>

#include "core/headers/Defender.h"
#include "core/headers/Goalkeeper.h"
#include "core/headers/Midfielder.h"
#include "core/headers/Striker.h"

namespace {
void addTestSquad(const std::shared_ptr<Team> &team)
{
    if (!team) {
        return;
    }

    team->addPlayer(std::make_shared<Goalkeeper>("GK", "Pawel", 92.0, 82, 87));
    team->addPlayer(std::make_shared<Defender>("CB", "Michal", 88.0, 80, 84));
    team->addPlayer(std::make_shared<Defender>("RB", "Kamil", 86.0, 79, 82));
    team->addPlayer(std::make_shared<Midfielder>("CM", "Jakub", 89.0, 81, 85));
    team->addPlayer(std::make_shared<Midfielder>("CAM", "Adrian", 87.0, 80, 83));
    team->addPlayer(std::make_shared<Striker>("ST", "Piotr", 91.0, 83, 86));
    team->addPlayer(std::make_shared<Striker>("LW", "Tomasz", 85.0, 78, 81));
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , leagueTabs(nullptr)
    , setupPhase(SetupPhase::Configuration)
{
    ui->setupUi(this);
    setupStartPanel();
    setupLeagueViews();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<std::string> MainWindow::getCheckedTeams() const
{
    std::vector<std::string> selected;
    for (int i = 0; i < ui->teamsPoolList->count(); ++i) {
        auto *item = ui->teamsPoolList->item(i);
        if (item && item->checkState() == Qt::Checked) {
            selected.push_back(item->text().toStdString());
        }
    }
    return selected;
}

void MainWindow::refreshTeamSelectionControls()
{
    const int requiredTeams = ui->teamsCountSpinBox->value();
    auto checkedTeams = getCheckedTeams();

    if (static_cast<int>(checkedTeams.size()) > requiredTeams) {
        int toUncheck = static_cast<int>(checkedTeams.size()) - requiredTeams;
        for (int i = ui->teamsPoolList->count() - 1; i >= 0 && toUncheck > 0; --i) {
            auto *item = ui->teamsPoolList->item(i);
            if (item && item->checkState() == Qt::Checked) {
                item->setCheckState(Qt::Unchecked);
                --toUncheck;
            }
        }
        checkedTeams = getCheckedTeams();
    }

    ui->playerTeamComboBox->clear();
    for (const auto& name : checkedTeams) {
        ui->playerTeamComboBox->addItem(QString::fromStdString(name));
    }

    const QString status = QString("Zaznaczone druzyny: %1 / %2")
                               .arg(checkedTeams.size())
                               .arg(requiredTeams);
    ui->setupStatusLabel->setText(status);
}

void MainWindow::setupStartPanel()
{
    setWindowTitle("Fifa26 Simulator - panel startowy");

    availableTeamPool = {
        "FC Krakow", "Legia Warszawa", "Lech Poznan", "Warta Poznan",
        "Pogon Szczecin", "Rakow Czestochowa", "Gornik Zabrze", "Jagiellonia Bialystok",
        "Wisla Krakow", "Cracovia", "Piast Gliwice", "Korona Kielce",
        "Stal Mielec", "Radomiak", "Zaglebie Lubin", "Slask Wroclaw"
    };

    ui->leagueNameEdit->setText("Ekstraklasa Test");
    ui->teamsCountSpinBox->setValue(4);
    ui->randomnessSpinBox->setValue(50);

    ui->teamsPoolList->clear();
    for (int i = 0; i < static_cast<int>(availableTeamPool.size()); ++i) {
        auto *item = new QListWidgetItem(QString::fromStdString(availableTeamPool[static_cast<size_t>(i)]));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(i < 4 ? Qt::Checked : Qt::Unchecked);
        ui->teamsPoolList->addItem(item);
    }

    ui->teamsCountSpinBox->setMaximum(static_cast<int>(availableTeamPool.size()));

    refreshTeamSelectionControls();

    connect(ui->prepareLeagueButton, &QPushButton::clicked, this, [this]() {
        prepareLeagueData();
    });

    connect(ui->teamsCountSpinBox, &QSpinBox::valueChanged, this, [this](int value) {
        if (value % 2 != 0) {
            ui->teamsCountSpinBox->setValue(value + 1);
            return;
        }
        refreshTeamSelectionControls();
    });

    connect(ui->teamsPoolList, &QListWidget::itemChanged, this, [this](QListWidgetItem *) {
        refreshTeamSelectionControls();
    });
}

void MainWindow::setupLeagueViews()
{
    leagueTabs = new QTabWidget(this);
    leagueTabs->setVisible(false);

    leagueTableWidget = std::make_unique<LeagueTableWidget>(leagueTabs);
    matchScheduleWidget = std::make_unique<MatchScheduleWidget>(leagueTabs);
    teamDetailsWidget = std::make_unique<TeamDetailsWidget>(leagueTabs);
    playerDetailsWidget = std::make_unique<PlayerDetailsWidget>(leagueTabs);

    leagueTabs->addTab(leagueTableWidget.get(), "Tabela");
    leagueTabs->addTab(matchScheduleWidget.get(), "Terminarz");
    leagueTabs->addTab(teamDetailsWidget.get(), "Szczegoly druzyny");
    leagueTabs->addTab(playerDetailsWidget.get(), "Szczegoly zawodnika");

    ui->verticalLayout->insertWidget(1, leagueTabs);

    connect(ui->abortSimulationButton, &QPushButton::clicked, this, [this]() {
        returnToSetupView();
    });

    connect(teamDetailsWidget.get(), &TeamDetailsWidget::phaseAdvanceRequested, this, [this]() {
        advanceToSchedulePhase();
    });

    connect(matchScheduleWidget.get(), &MatchScheduleWidget::phaseAdvanceRequested, this, [this]() {
        advanceToLeaguePhase();
    });

    connect(leagueTableWidget.get(), &LeagueTableWidget::simulateNextRequested, this, [this]() {
        simulateNextMatchday();
    });

    connect(leagueTableWidget.get(), &LeagueTableWidget::teamSelected, this, [this](const QString &teamName) {
        if (!leagueTabs || !teamDetailsWidget) {
            return;
        }
        teamDetailsWidget->selectTeamByName(teamName);
        leagueTabs->setCurrentWidget(teamDetailsWidget.get());
    });

    connect(teamDetailsWidget.get(), &TeamDetailsWidget::lineupEdited, this, [this]() {
        refreshLeagueView();
    });

    connect(teamDetailsWidget.get(), &TeamDetailsWidget::playerSelectedRequested,
            this,
            [this](const QString& teamName, int playerIndex) {
                if (!leagueEngine || !playerDetailsWidget || !leagueTabs) {
                    return;
                }

                for (const auto& team : leagueEngine->getTeams()) {
                    if (QString::fromStdString(team->getName()) != teamName) {
                        continue;
                    }

                    const auto& players = team->getPlayers();
                    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
                        return;
                    }

                    playerDetailsWidget->displayPlayerDetails(players[static_cast<size_t>(playerIndex)], team);
                    leagueTabs->setCurrentWidget(playerDetailsWidget.get());
                    return;
                }
            });

    connect(teamDetailsWidget.get(), &TeamDetailsWidget::transferRequested,
            this,
            [this](const QString& fromTeam,
                   const QString& toTeam,
                   const QString& playerName,
                   const QString& playerSurname) {
                if (!leagueEngine) {
                    return;
                }

                const bool ok = leagueEngine->transferPlayer(fromTeam.toStdString(),
                                                             toTeam.toStdString(),
                                                             playerName.toStdString(),
                                                             playerSurname.toStdString());
                ui->setupStatusLabel->setText(ok ? "Transfer zakonczony sukcesem." : "Transfer odrzucony.");
                refreshLeagueView();
            });

    connect(matchScheduleWidget.get(), &MatchScheduleWidget::matchSlotEditRequested,
            this,
            [this](int matchdayIndex, int matchIndex, const QString& slot) {
                if (!leagueEngine) {
                    return;
                }

                const bool ok = leagueEngine->updateMatchSlot(static_cast<size_t>(matchdayIndex),
                                                              static_cast<size_t>(matchIndex),
                                                              slot.toStdString());
                ui->setupStatusLabel->setText(ok ? "Zmieniono termin meczu." : "Nie udalo sie zmienic terminu.");
                refreshLeagueView();
            });
}

void MainWindow::prepareLeagueData()
{
    const int requiredTeams = ui->teamsCountSpinBox->value();
    auto selectedTeamNames = getCheckedTeams();

    if (requiredTeams < 2 || requiredTeams % 2 != 0) {
        QMessageBox::warning(this, "Bledna konfiguracja", "Liczba druzyn musi byc parzysta i >= 2.");
        return;
    }

    if (static_cast<int>(selectedTeamNames.size()) != requiredTeams) {
        QMessageBox::warning(this,
                             "Bledna konfiguracja",
                             QString("Zaznacz dokladnie %1 druzyn.").arg(requiredTeams));
        return;
    }

    leagueEngine = std::make_unique<LeagueEngine>();
    leagueEngine->setRandomnessPercent(ui->randomnessSpinBox->value());

    for (int i = 0; i < requiredTeams; ++i) {
        auto team = std::make_shared<Team>(selectedTeamNames[static_cast<size_t>(i)]);
        addTestSquad(team);
        team->setDefaultLineup();
        leagueEngine->addTeam(team);
    }

    leagueEngine->setPlayerTeam(ui->playerTeamComboBox->currentText().toStdString());
    leagueEngine->generateSchedule();

    const QString status = QString("Liga '%1' gotowa. Liczba druzyn: %2. Kolejek: %3. Losowosc: %4")
                               .arg(ui->leagueNameEdit->text())
                               .arg(requiredTeams)
                               .arg(leagueEngine->getTotalMatchdays())
                               .arg(leagueEngine->getRandomnessPercent());
    ui->setupStatusLabel->setText(status);

    ui->configGroupBox->setVisible(false);
    ui->abortSimulationButton->setVisible(true);
    leagueTabs->setVisible(true);
    refreshLeagueView();

    advanceToTransfersPhase();
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
        leagueEngine->isLeagueFinished(),
        leagueEngine->getSchedule(),
        leagueEngine->getPlayerTeam());

    matchScheduleWidget->refreshSchedule(
        leagueEngine->getSchedule(),
        leagueEngine->getCurrentMatchday());

    teamDetailsWidget->refreshData(
        leagueEngine->getTeams(),
        leagueEngine->getPlayerTeam(),
        leagueEngine->getSchedule());

    playerDetailsWidget->refreshPlayers(
        leagueEngine->getTeams(),
        leagueEngine->getPlayerTeam());
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

void MainWindow::returnToSetupView()
{
    leagueTabs->setVisible(false);
    ui->configGroupBox->setVisible(true);
    ui->abortSimulationButton->setVisible(false);
    leagueEngine.reset();
    setupPhase = SetupPhase::Configuration;
    setWindowTitle("Fifa26 Simulator - panel startowy");
    ui->setupStatusLabel->setText("Liga zresetowana. Mozesz zaczac od nowa.");
}

void MainWindow::advanceToTransfersPhase()
{
    if (setupPhase != SetupPhase::Configuration) return;
    setupPhase = SetupPhase::Transfers;
    ui->setupStatusLabel->setText("Faza B: Transfery. Mozesz przeprowadzic transfery dla swoich druzyn.");
    leagueTabs->setCurrentIndex(2);
    leagueTabs->setTabEnabled(0, false);
    leagueTabs->setTabEnabled(1, false);
    leagueTabs->setTabEnabled(2, true);
}

void MainWindow::advanceToSchedulePhase()
{
    if (setupPhase != SetupPhase::Transfers) return;
    setupPhase = SetupPhase::Schedule;
    ui->setupStatusLabel->setText("Faza C: Edycja terminarza. Mozesz zmieniaz terminy i drag&drop meczow.");
    leagueTabs->setCurrentIndex(1);
    leagueTabs->setTabEnabled(1, true);
}

void MainWindow::advanceToLeaguePhase()
{
    if (setupPhase != SetupPhase::Schedule) return;
    setupPhase = SetupPhase::League;
    ui->setupStatusLabel->setText("Liga rozpoczeta! Transfery i edycja terminarza sa blokowane.");
    leagueTabs->setCurrentIndex(0);
    leagueTabs->setTabEnabled(0, true);
    leagueTabs->setTabEnabled(1, true);
    leagueTabs->setTabEnabled(2, true);

    teamDetailsWidget->setTransferEditorEnabled(false);
    matchScheduleWidget->setEditingEnabled(false);
}

