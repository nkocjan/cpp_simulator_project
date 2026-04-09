#include "PlayerDetailsWidget.h"

#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>

PlayerDetailsWidget::PlayerDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , playerSelector(nullptr)
    , nameLabel(nullptr)
    , positionLabel(nullptr)
    , overallLabel(nullptr)
    , conditionLabel(nullptr)
    , statusLabel(nullptr)
    , goalsLabel(nullptr)
    , assistsLabel(nullptr)
    , yellowCardsLabel(nullptr)
    , redCardsLabel(nullptr)
    , minutesPlayedLabel(nullptr)
    , injuryLabel(nullptr)
    , suspensionLabel(nullptr)
    , moraleLabel(nullptr)
    , statsTable(nullptr)
    , matchHistoryTable(nullptr)
{
    setupUi();
}

void PlayerDetailsWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Szczegóły zawodnika", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    auto *selectorGroup = new QGroupBox("Wybierz zawodnika", this);
    auto *selectorLayout = new QFormLayout(selectorGroup);
    playerSelector = new QComboBox(selectorGroup);
    playerSelector->addItem("-- wybierz zawodnika --");
    selectorLayout->addRow("Zawodnik:", playerSelector);
    layout->addWidget(selectorGroup);

    auto *playerInfoGroup = new QGroupBox("Dane zawodnika", this);
    auto *playerInfoLayout = new QFormLayout(playerInfoGroup);

    nameLabel = new QLabel("", this);
    positionLabel = new QLabel("", this);
    overallLabel = new QLabel("", this);
    conditionLabel = new QLabel("", this);
    statusLabel = new QLabel("", this);
    moraleLabel = new QLabel("", this);

    playerInfoLayout->addRow("Zawodnik:", nameLabel);
    playerInfoLayout->addRow("Pozycja:", positionLabel);
    playerInfoLayout->addRow("OVR:", overallLabel);
    playerInfoLayout->addRow("Kondycja:", conditionLabel);
    playerInfoLayout->addRow("Morale:", moraleLabel);
    playerInfoLayout->addRow("Status:", statusLabel);

    layout->addWidget(playerInfoGroup);

    auto *seasonStatsGroup = new QGroupBox("Statystyki sezonowe", this);
    auto *seasonLayout = new QFormLayout(seasonStatsGroup);

    goalsLabel = new QLabel("0", this);
    assistsLabel = new QLabel("0", this);
    yellowCardsLabel = new QLabel("0", this);
    redCardsLabel = new QLabel("0", this);
    minutesPlayedLabel = new QLabel("0", this);

    seasonLayout->addRow("Gole:", goalsLabel);
    seasonLayout->addRow("Asysty:", assistsLabel);
    seasonLayout->addRow("Żółte kartki:", yellowCardsLabel);
    seasonLayout->addRow("Czerwone kartki:", redCardsLabel);
    seasonLayout->addRow("Minuty grane:", minutesPlayedLabel);

    layout->addWidget(seasonStatsGroup);

    auto *availabilityGroup = new QGroupBox("Dostępność", this);
    auto *availLayout = new QFormLayout(availabilityGroup);

    injuryLabel = new QLabel("Brak", this);
    suspensionLabel = new QLabel("Brak", this);

    availLayout->addRow("Kontuzja:", injuryLabel);
    availLayout->addRow("Zawieszenie:", suspensionLabel);

    layout->addWidget(availabilityGroup);

    auto *statsTitle = new QLabel("Statystyki zawodnika", this);
    QFont statsFont = statsTitle->font();
    statsFont.setBold(true);
    statsTitle->setFont(statsFont);
    layout->addWidget(statsTitle);

    statsTable = new QTableWidget(this);
    statsTable->setColumnCount(2);
    statsTable->setHorizontalHeaderLabels({"Statystyka", "Wartość"});
    statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statsTable->setMaximumHeight(200);
    layout->addWidget(statsTable);

    auto *historyTitle = new QLabel("Historia meczów", this);
    historyTitle->setFont(statsFont);
    layout->addWidget(historyTitle);

    matchHistoryTable = new QTableWidget(this);
    matchHistoryTable->setColumnCount(6);
    matchHistoryTable->setHorizontalHeaderLabels({"Kolejka", "Przeciwnik", "G", "A", "Kartki", "Minuty"});
    matchHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(matchHistoryTable);

    layout->addStretch();

    connect(playerSelector, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index <= 0) {
            return;
        }

        const QString key = playerSelector->currentData().toString();
        const QStringList parts = key.split(':');
        if (parts.size() != 2) {
            return;
        }

        const int teamIndex = parts[0].toInt();
        const int playerIndex = parts[1].toInt();
        if (teamIndex < 0 || teamIndex >= static_cast<int>(teamsCache.size())) {
            return;
        }

        const auto& team = teamsCache[static_cast<size_t>(teamIndex)];
        const auto& players = team->getPlayers();
        if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
            return;
        }

        displayPlayerDetails(players[static_cast<size_t>(playerIndex)], team);
    });
}

void PlayerDetailsWidget::refreshPlayers(const std::vector<std::shared_ptr<Team>>& teams,
                                         const std::shared_ptr<Team>& preferredTeam)
{
    teamsCache = teams;
    const QString previousKey = playerSelector->currentData().toString();

    playerSelector->blockSignals(true);
    playerSelector->clear();
    playerSelector->addItem("-- wybierz zawodnika --");

    int preferredIndex = -1;
    for (int t = 0; t < static_cast<int>(teamsCache.size()); ++t) {
        const auto& team = teamsCache[static_cast<size_t>(t)];
        const auto& players = team->getPlayers();
        for (int p = 0; p < static_cast<int>(players.size()); ++p) {
            const auto& player = players[static_cast<size_t>(p)];
            const QString text = QString("%1 %2 (%3)")
                                     .arg(QString::fromStdString(player->getName()))
                                     .arg(QString::fromStdString(player->getSurname()))
                                     .arg(QString::fromStdString(team->getName()));
            const QString key = QString("%1:%2").arg(t).arg(p);
            playerSelector->addItem(text, key);

            if (preferredIndex < 0 && preferredTeam && team == preferredTeam) {
                preferredIndex = playerSelector->count() - 1;
            }
        }
    }

    int targetIndex = previousKey.isEmpty() ? -1 : playerSelector->findData(previousKey);
    if (targetIndex < 0 && preferredIndex >= 0) {
        targetIndex = preferredIndex;
    }
    if (targetIndex < 0 && playerSelector->count() > 1) {
        targetIndex = 1;
    }

    playerSelector->setCurrentIndex(targetIndex >= 0 ? targetIndex : 0);
    playerSelector->blockSignals(false);

    if (playerSelector->currentIndex() > 0) {
        const int idx = playerSelector->currentIndex();
        const QString key = playerSelector->itemData(idx).toString();
        const QStringList parts = key.split(':');
        if (parts.size() == 2) {
            const int teamIndex = parts[0].toInt();
            const int playerIndex = parts[1].toInt();
            if (teamIndex >= 0 && teamIndex < static_cast<int>(teamsCache.size())) {
                const auto& team = teamsCache[static_cast<size_t>(teamIndex)];
                const auto& players = team->getPlayers();
                if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
                    displayPlayerDetails(players[static_cast<size_t>(playerIndex)], team);
                }
            }
        }
    }
}

void PlayerDetailsWidget::displayPlayerDetails(const std::shared_ptr<Player>& player,
                                               const std::shared_ptr<Team>& team)
{
    if (!player) {
        nameLabel->setText("Brak zawodnika do wyświetlenia");
        return;
    }

    nameLabel->setText(QString::fromStdString(player->getName() + " " + player->getSurname()) +
                       (team ? QString(" (%1)").arg(QString::fromStdString(team->getName())) : QString()));

    auto getPositionString = [](PlayerPosition pos) -> QString {
        switch (pos) {
        case PlayerPosition::Goalkeeper: return "Bramkarz";
        case PlayerPosition::Defender: return "Obrońca";
        case PlayerPosition::Midfielder: return "Pomocnik";
        case PlayerPosition::Striker: return "Napastnik";
        }
        return "Nieznana";
    };

    positionLabel->setText(getPositionString(player->getNaturalPosition()));
    overallLabel->setText(QString::number(player->getOverall()));
    conditionLabel->setText(QString::number(player->getCondition(), 'f', 1) + "%");
    moraleLabel->setText(QString::number(player->getMorale(), 'f', 1));

    if (!player->isAvailable()) {
        QString status;
        if (player->getInjuryDuration() > 0) {
            status += QString("Kontuzja: %1 meczów").arg(player->getInjuryDuration());
        }
        if (player->getSuspensionDuration() > 0) {
            if (!status.isEmpty()) status += ", ";
            status += QString("Zawieszenie: %1 meczów").arg(player->getSuspensionDuration());
        }
        statusLabel->setText(status);
        statusLabel->setStyleSheet("color: red;");
    } else {
        statusLabel->setText("Dostępny");
        statusLabel->setStyleSheet("color: green;");
    }

    goalsLabel->setText(QString::number(player->getGoals()));
    assistsLabel->setText(QString::number(player->getAssists()));
    yellowCardsLabel->setText(QString::number(player->getYellowCards()));
    redCardsLabel->setText(QString::number(player->getRedCards()));
    minutesPlayedLabel->setText(QString::number(player->getMinutesPlayed()));

    if (player->getInjuryDuration() > 0) {
        injuryLabel->setText(QString("Pauzuje przez %1 meczów").arg(player->getInjuryDuration()));
        injuryLabel->setStyleSheet("color: red;");
    } else {
        injuryLabel->setText("Brak");
        injuryLabel->setStyleSheet("color: green;");
    }

    if (player->getSuspensionDuration() > 0) {
        suspensionLabel->setText(QString("Zawieszony przez %1 meczów").arg(player->getSuspensionDuration()));
        suspensionLabel->setStyleSheet("color: orange;");
    } else {
        suspensionLabel->setText("Brak");
        suspensionLabel->setStyleSheet("color: green;");
    }

    updateStats(player);
}

void PlayerDetailsWidget::updateStats(const std::shared_ptr<Player>& player)
{
    if (!player) return;

    statsTable->setRowCount(0);

    auto addStat = [this](const QString& name, const QString& value) {
        int row = statsTable->rowCount();
        statsTable->insertRow(row);
        statsTable->setItem(row, 0, new QTableWidgetItem(name));
        statsTable->setItem(row, 1, new QTableWidgetItem(value));
    };

    addStat("Ocena ogólna", QString::number(player->getOverall()));
    addStat("Kondycja", QString::number(player->getCondition(), 'f', 1) + "%");
    addStat("Morale", QString::number(player->getMorale(), 'f', 1));
    addStat("Gole", QString::number(player->getGoals()));
    addStat("Asysty", QString::number(player->getAssists()));
    addStat("Żółte kartki", QString::number(player->getYellowCards()));
    addStat("Czerwone kartki", QString::number(player->getRedCards()));
    addStat("Minuty grane", QString::number(player->getMinutesPlayed()));

    matchHistoryTable->setRowCount(0);
}
