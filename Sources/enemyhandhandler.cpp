#include "enemyhandhandler.h"
#include <QtWidgets>

EnemyHandHandler::EnemyHandHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->inGame = false;
    this->transparency = Opaque;
    this->knownCard = "";
    this->numKnownCards = 0;
    this->lastCreatedByCode = "";

    completeUI();
}


EnemyHandHandler::~EnemyHandHandler()
{
    ui->enemyHandListWidget->clear();
    enemyHandList.clear();
}


void EnemyHandHandler::reset()
{
    ui->enemyHandListWidget->clear();
    enemyHandList.clear();
}


void EnemyHandHandler::completeUI()
{
    ui->enemyHandListWidget->setMinimumHeight(0);
    ui->enemyHandListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    ui->enemyHandListWidget->setMouseTracking(true);

    connect(ui->enemyHandListWidget, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(findHandCardEntered(QListWidgetItem*)));
}


void EnemyHandHandler::setLastCreatedByCode(QString code)
{
    this->lastCreatedByCode = code;
}


void EnemyHandHandler::convertDuplicates(QString code)
{
    convertKnownCard(code, 2);
}
void EnemyHandHandler::convertKnownCard(QString &code, int quantity)
{
    this->knownCard = code;
    this->numKnownCards = quantity;
}


void EnemyHandHandler::showEnemyCardDraw(int id, int turn, bool special, QString code)
{
    bool isCreatedByCard = false;

    //Convert Known special cards
    if(!special)
    {
        knownCard.clear();
        numKnownCards = 0;
    }
    else if(code.isEmpty())
    {
        if(numKnownCards>0)
        {
            code = knownCard;
            numKnownCards--;
        }
        else if(!this->lastCreatedByCode.isEmpty())
        {
            isCreatedByCard = true;
        }
    }


    HandCard handCard(code);
    handCard.id = id;
    handCard.turn = turn;
    handCard.special = special;
    handCard.listItem = new QListWidgetItem();
    ui->enemyHandListWidget->addItem(handCard.listItem);

    if(isCreatedByCard)
    {
        handCard.setCreatedByCode(this->lastCreatedByCode);
        emit checkCardImage(this->lastCreatedByCode);
    }

    handCard.draw();
    enemyHandList.append(handCard);

    if(code != "")      emit checkCardImage(code);
}


void EnemyHandHandler::lastHandCardIsCoin()
{
    if(enemyHandList.empty())   return;//En modo practica el mulligan enemigo termina antes de robar las cartas
    enemyHandList.last().setCode(COIN);
    enemyHandList.last().draw();

    emit checkCardImage(COIN);
}


void EnemyHandHandler::hideEnemyCardPlayed(int id, QString code)
{
    (void) code;

    int i=0;
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++, i++)
    {
        if(it->id == id)
        {
            delete it->listItem;
            enemyHandList.removeAt(i);
            return;
        }
    }
}


void EnemyHandHandler::redrawDownloadedCardImage(QString &code)
{
    for (QList<HandCard>::iterator it = enemyHandList.begin(); it != enemyHandList.end(); it++)
    {
        if(it->getCode() == code || it->getCreatedByCode() == code)    it->draw();
    }
}


void EnemyHandHandler::lockEnemyInterface()
{
    this->inGame = true;
    updateTransparency();

    reset();
}


void EnemyHandHandler::unlockEnemyInterface()
{
    this->inGame = false;
    updateTransparency();
}


void EnemyHandHandler::updateTransparency()
{
    if(transparency==Transparent || (inGame && transparency==AutoTransparent))
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground);
        ui->tabEnemy->repaint();
    }
    else
    {
        ui->tabEnemy->setAttribute(Qt::WA_NoBackground, false);
        ui->tabEnemy->repaint();
    }
}


void EnemyHandHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void EnemyHandHandler::findHandCardEntered(QListWidgetItem * item)
{
    HandCard handCard = enemyHandList[ui->enemyHandListWidget->row(item)];
    QString code = handCard.getCode();
    if(code.isEmpty())  code = handCard.getCreatedByCode();

    QRect rectCard = ui->enemyHandListWidget->visualItemRect(item);
    QPoint posCard = ui->enemyHandListWidget->mapToGlobal(rectCard.topLeft());
    QRect globalRectCard = QRect(posCard, rectCard.size());

    int enemyHandListTop = ui->enemyHandListWidget->mapToGlobal(QPoint(0,0)).y();
    int enemyHandListBottom = ui->enemyHandListWidget->mapToGlobal(QPoint(0,ui->enemyHandListWidget->height())).y();
    emit cardEntered(code, globalRectCard, enemyHandListTop, enemyHandListBottom);
}



