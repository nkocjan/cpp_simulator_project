#include "TeamDetailsWidget.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QString positionToString(PlayerPosition pos) {
    switch (pos) {
    case PlayerPosition::Goalkeeper: return "Bramkarz";
    case PlayerPosition::Defender: return "Obronca";
    case PlayerPosition::Midfielder: return "Pomocnik";
    case PlayerPosition::Striker: return "Napastnik";
    }
    return "?";
}
}

TeamDetailsWidget::TeamDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , teamSelector(nullptr)
    , teamStatsLabel(nullptr)
    , playersTable(nullptr)
    , historyList(nullptr)
    , upcomingList(nullptr)
    , unavailableList(nullptr)
    , gkStarterCombo(nullptr)
    , defStarterCombo(nullptr)
    , midStarterCombo(nullptr)
    , stStarterCombo(nullptr)
    , applyLineupButton(nullptr)
    , lineupEditorInfoLabel(nullptr)
    , transferFromTeamCombo(nullptr)
    , transferPlayerCombo(nullptr)
    , transferButton(nullptr)
    , confirmTransfersButton(nullptr)
    , transferInfoLabel(nullptr)
    , transferEditorEnabled(true)
{
    setupUi();
}

void TeamDetailsWidget::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    auto *content = new QWidget(scrollArea);
    auto *layout = new QVBoxLayout(content);

    auto *title = new QLabel("Szczegoly druzyny", content);
    QFont titleFont = title->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    title->setFont(titleFont);

    teamSelector = new QComboBox(content);
    teamStatsLabel = new QLabel("Wybierz druzyne.", content);

    playersTable = new QTableWidget(content);
    playersTable->setColumnCount(9);
    playersTable->setHorizontalHeaderLabels({"Imie", "Nazwisko", "Pozycja", "OVR", "Kond.", "G", "A", "Kartki", "Min"});
    playersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    playersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    playersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    playersTable->setMinimumHeight(300);

    auto *lineupGroup = new QGroupBox("Edycja skladu (playerTeam)", content);
    auto *lineupForm = new QFormLayout(lineupGroup);

    gkStarterCombo = new QComboBox(lineupGroup);
    defStarterCombo = new QComboBox(lineupGroup);
    midStarterCombo = new QComboBox(lineupGroup);
    stStarterCombo = new QComboBox(lineupGroup);
    applyLineupButton = new QPushButton("Zapisz sklad", lineupGroup);
    lineupEditorInfoLabel = new QLabel("Edytor aktywny tylko dla Twojej druzyny.", lineupGroup);

    lineupForm->addRow("Bramkarz:", gkStarterCombo);
    lineupForm->addRow("Obronca:", defStarterCombo);
    lineupForm->addRow("Pomocnik:", midStarterCombo);
    lineupForm->addRow("Napastnik:", stStarterCombo);
    lineupForm->addRow(applyLineupButton);
    lineupForm->addRow(lineupEditorInfoLabel);

    auto *transferGroup = new QGroupBox("Transfery", content);
    auto *transferForm = new QFormLayout(transferGroup);
    transferFromTeamCombo = new QComboBox(transferGroup);
    transferPlayerCombo = new QComboBox(transferGroup);
    transferButton = new QPushButton("Sprowadz zawodnika", transferGroup);
    transferInfoLabel = new QLabel("Transfery dostepne dla playerTeam.", transferGroup);

    transferForm->addRow("Klub zrodlowy:", transferFromTeamCombo);
    transferForm->addRow("Zawodnik:", transferPlayerCombo);
    transferForm->addRow(transferButton);
    confirmTransfersButton = new QPushButton("Zatwierdz transfery -> Terminarz", transferGroup);
    transferForm->addRow(confirmTransfersButton);
    transferForm->addRow(transferInfoLabel);

    auto *unavailableTitle = new QLabel("Niedostepni (kontuzja/zawieszenie):", content);
    unavailableList = new QListWidget(content);

    auto *historyTitle = new QLabel("Historia meczow:", content);
    historyList = new QListWidget(content);

    auto *upcomingTitle = new QLabel("Nastepne spotkania:", content);
    upcomingList = new QListWidget(content);

    layout->addWidget(title);
    layout->addWidget(teamSelector);
    layout->addWidget(teamStatsLabel);
    layout->addWidget(playersTable);
    layout->addWidget(lineupGroup);
    layout->addWidget(transferGroup);
    layout->addWidget(unavailableTitle);
    layout->addWidget(unavailableList);
    layout->addWidget(historyTitle);
    layout->addWidget(historyList);
    layout->addWidget(upcomingTitle);
    layout->addWidget(upcomingList);

    content->setLayout(layout);
    scrollArea->setWidget(content);
    rootLayout->addWidget(scrollArea);

    connect(playersTable, &QTableWidget::cellClicked, this, [this](int row, int) {
        const int teamIndex = teamSelector ? teamSelector->currentIndex() : -1;
        if (teamIndex < 0 || teamIndex >= static_cast<int>(teamsCache.size())) {
            return;
        }

        const auto& team = teamsCache[static_cast<size_t>(teamIndex)];
        const auto& players = team->getPlayers();
        if (row < 0 || row >= static_cast<int>(players.size())) {
            return;
        }

        emit playerSelectedRequested(QString::fromStdString(team->getName()), row);
    });

    connect(teamSelector, &QComboBox::currentIndexChanged, this, [this]() {
        renderSelectedTeam();
    });

    connect(transferFromTeamCombo, &QComboBox::currentIndexChanged, this, [this]() {
        transferPlayerCombo->clear();
        const QString sourceName = transferFromTeamCombo->currentText();
        if (sourceName.isEmpty()) {
            return;
        }

        for (const auto& team : teamsCache) {
            if (QString::fromStdString(team->getName()) != sourceName) {
                continue;
            }

            const auto& players = team->getPlayers();
            for (int i = 0; i < static_cast<int>(players.size()); ++i) {
                const auto& p = players[static_cast<size_t>(i)];
                const QString playerText = QString("%1 %2 | %3 | OVR %4")
                                               .arg(QString::fromStdString(p->getName()))
                                               .arg(QString::fromStdString(p->getSurname()))
                                               .arg(positionToString(p->getNaturalPosition()))
                                               .arg(p->getOverall());
                transferPlayerCombo->addItem(playerText, i);
            }
            break;
        }
    });

    connect(transferButton, &QPushButton::clicked, this, [this]() {
        if (!playerTeamCache) {
            return;
        }

        const QString fromTeam = transferFromTeamCombo->currentText();
        const QString toTeam = QString::fromStdString(playerTeamCache->getName());
        if (fromTeam.isEmpty() || fromTeam == toTeam) {
            transferInfoLabel->setText("Wybierz poprawny klub zrodlowy.");
            return;
        }

        const int idx = transferPlayerCombo->currentData().toInt();
        for (const auto& team : teamsCache) {
            if (QString::fromStdString(team->getName()) != fromTeam) {
                continue;
            }
            const auto& players = team->getPlayers();
            if (idx < 0 || idx >= static_cast<int>(players.size())) {
                return;
            }
            const auto& player = players[static_cast<size_t>(idx)];
            emit transferRequested(fromTeam,
                                   toTeam,
                                   QString::fromStdString(player->getName()),
                                   QString::fromStdString(player->getSurname()));
            transferInfoLabel->setText("Transfer zakonczony.");
            break;
        }
    });

    connect(confirmTransfersButton, &QPushButton::clicked, this, [this]() {
        emit phaseAdvanceRequested();
    });

    connect(applyLineupButton, &QPushButton::clicked, this, [this]() {
        const int index = teamSelector->currentIndex();
        if (index < 0 || index >= static_cast<int>(teamsCache.size())) {
            return;
        }

        const auto& team = teamsCache[static_cast<size_t>(index)];
        if (!playerTeamCache || team != playerTeamCache) {
            return;
        }

        const auto gk = comboSelectionToPlayer(gkStarterCombo, team);
        const auto def = comboSelectionToPlayer(defStarterCombo, team);
        const auto mid = comboSelectionToPlayer(midStarterCombo, team);
        const auto st = comboSelectionToPlayer(stStarterCombo, team);

        bool ok = true;
        ok = team->setStarter(PlayerPosition::Goalkeeper, gk) && ok;
        ok = team->setStarter(PlayerPosition::Defender, def) && ok;
        ok = team->setStarter(PlayerPosition::Midfielder, mid) && ok;
        ok = team->setStarter(PlayerPosition::Striker, st) && ok;

        if (ok) {
            lineupEditorInfoLabel->setText("Sklad zapisany.");
            emit lineupEdited();
            renderSelectedTeam();
        } else {
            lineupEditorInfoLabel->setText("Nie udalo sie zapisac skladu.");
        }
    });
}

void TeamDetailsWidget::refreshData(const std::vector<std::shared_ptr<Team>> &teams,
                                    const std::shared_ptr<Team> &preferredTeam,
                                    const std::vector<std::vector<std::shared_ptr<Match>>> &schedule)
{
    teamsCache = teams;
    scheduleCache = schedule;
    playerTeamCache = preferredTeam;

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

std::shared_ptr<Player> TeamDetailsWidget::comboSelectionToPlayer(QComboBox *combo, const std::shared_ptr<Team>& team) const
{
    if (!combo || !team) {
        return nullptr;
    }

    const int idx = combo->currentData().toInt();
    const auto& players = team->getPlayers();
    if (idx < 0 || idx >= static_cast<int>(players.size())) {
        return nullptr;
    }

    return players[static_cast<size_t>(idx)];
}

void TeamDetailsWidget::populateLineupEditor(const std::shared_ptr<Team>& team, bool isPlayerTeam)
{
    if (!team) return;

    auto fillCombo = [](QComboBox *combo, const std::shared_ptr<Team>& t, PlayerPosition pos) {
        combo->clear();
        const auto& players = t->getPlayers();
        int selectedIdx = -1;

        for (int i = 0; i < static_cast<int>(players.size()); ++i) {
            const auto& p = players[static_cast<size_t>(i)];
            const bool out = !p->isAvailable();
            const QString text = QString("%1 %2 | %3 | OVR %4%5")
                                     .arg(QString::fromStdString(p->getName()))
                                     .arg(QString::fromStdString(p->getSurname()))
                                     .arg(positionToString(p->getNaturalPosition()))
                                     .arg(p->getOverall())
                                     .arg(out ? " | OUT" : "");
            combo->addItem(text, i);

            const auto starter = t->getStarter(pos);
            if (starter && starter == p) {
                selectedIdx = i;
            }
        }

        if (selectedIdx >= 0) {
            combo->setCurrentIndex(selectedIdx);
        }
    };

    fillCombo(gkStarterCombo, team, PlayerPosition::Goalkeeper);
    fillCombo(defStarterCombo, team, PlayerPosition::Defender);
    fillCombo(midStarterCombo, team, PlayerPosition::Midfielder);
    fillCombo(stStarterCombo, team, PlayerPosition::Striker);

    gkStarterCombo->setEnabled(isPlayerTeam);
    defStarterCombo->setEnabled(isPlayerTeam);
    midStarterCombo->setEnabled(isPlayerTeam);
    stStarterCombo->setEnabled(isPlayerTeam);
    applyLineupButton->setEnabled(isPlayerTeam);

    lineupEditorInfoLabel->setText(isPlayerTeam
                                   ? "Mozesz edytowac sklad swojej druzyny."
                                   : "Edytor aktywny tylko dla Twojej druzyny.");
}

void TeamDetailsWidget::setTransferEditorEnabled(bool enabled)
{
    transferEditorEnabled = enabled;

    const int index = teamSelector ? teamSelector->currentIndex() : -1;
    const bool hasTeam = index >= 0 && index < static_cast<int>(teamsCache.size());
    const bool isPlayerTeam = hasTeam && playerTeamCache && teamsCache[static_cast<size_t>(index)] == playerTeamCache;

    populateTransferEditor(hasTeam ? teamsCache[static_cast<size_t>(index)] : nullptr, isPlayerTeam);
}

void TeamDetailsWidget::populateTransferEditor(const std::shared_ptr<Team>& selectedTeam, bool isPlayerTeam)
{
    transferFromTeamCombo->clear();
    transferPlayerCombo->clear();

    const bool enable = transferEditorEnabled && isPlayerTeam && static_cast<bool>(playerTeamCache);
    transferFromTeamCombo->setEnabled(enable);
    transferPlayerCombo->setEnabled(enable);
    transferButton->setEnabled(enable);
    confirmTransfersButton->setEnabled(enable);

    if (!enable || !selectedTeam) {
        transferInfoLabel->setText("Transfery dostepne dla playerTeam.");
        return;
    }

    for (const auto& team : teamsCache) {
        if (team == selectedTeam) {
            continue;
        }
        transferFromTeamCombo->addItem(QString::fromStdString(team->getName()));
    }

    if (transferFromTeamCombo->count() > 0) {
        transferFromTeamCombo->setCurrentIndex(0);
    }

    transferInfoLabel->setText("Wybierz zawodnika do transferu.");
}

void TeamDetailsWidget::renderSelectedTeam()
{
    const int index = teamSelector->currentIndex();
    if (index < 0 || index >= static_cast<int>(teamsCache.size())) {
        teamStatsLabel->setText("Brak danych druzyny.");
        playersTable->setRowCount(0);
        unavailableList->clear();
        historyList->clear();
        upcomingList->clear();
        return;
    }

    const auto &team = teamsCache[static_cast<size_t>(index)];
    const bool isPlayerTeam = (playerTeamCache && team == playerTeamCache);

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
        playersTable->setItem(i, 2, new QTableWidgetItem(positionToString(player->getNaturalPosition())));
        playersTable->setItem(i, 3, new QTableWidgetItem(QString::number(player->getOverall())));
        playersTable->setItem(i, 4, new QTableWidgetItem(QString::number(player->getCondition(), 'f', 1)));
        playersTable->setItem(i, 5, new QTableWidgetItem(QString::number(player->getGoals())));
        playersTable->setItem(i, 6, new QTableWidgetItem(QString::number(player->getAssists())));
        playersTable->setItem(i, 7, new QTableWidgetItem(cards));
        playersTable->setItem(i, 8, new QTableWidgetItem(QString::number(player->getMinutesPlayed())));
    }

    populateLineupEditor(team, isPlayerTeam);
    populateTransferEditor(team, isPlayerTeam);

    unavailableList->clear();
    for (const auto& player : players) {
        if (!player->isAvailable()) {
            QString reason;
            if (player->getInjuryDuration() > 0) {
                reason += QString("kontuzja: %1").arg(player->getInjuryDuration());
            }
            if (player->getSuspensionDuration() > 0) {
                if (!reason.isEmpty()) {
                    reason += ", ";
                }
                reason += QString("zawieszenie: %1").arg(player->getSuspensionDuration());
            }
            unavailableList->addItem(QString("%1 %2 (%3)")
                                         .arg(QString::fromStdString(player->getName()))
                                         .arg(QString::fromStdString(player->getSurname()))
                                         .arg(reason));
        }
    }

    historyList->clear();
    upcomingList->clear();

    for (int day = 0; day < static_cast<int>(scheduleCache.size()); ++day) {
        for (const auto &match : scheduleCache[static_cast<size_t>(day)]) {
            const bool isTeamMatch = (match->getHomeTeam() == team) || (match->getAwayTeam() == team);
            if (!isTeamMatch) {
                continue;
            }

            const QString line = QString("K%1: %2 vs %3 (%4)")
                                     .arg(day + 1)
                                     .arg(QString::fromStdString(match->getHomeTeam()->getName()))
                                     .arg(QString::fromStdString(match->getAwayTeam()->getName()))
                                     .arg(QString::fromStdString(match->getWeekendSlot()));

            if (match->getIsFinished()) {
                historyList->addItem(line + QString(" | %1:%2").arg(match->getHomeGoals()).arg(match->getAwayGoals()));
            } else {
                upcomingList->addItem(line);
            }
        }
    }
}
