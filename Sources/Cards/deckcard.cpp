#include "deckcard.h"
#include "../utility.h"
#include <QtWidgets>


QMap<QString, QJsonObject> * DeckCard::cardsJson;


DeckCard::DeckCard(QString code)
{
    setCode(code);
    listItem = NULL;
    total = remaining = 1;
}


DeckCard::~DeckCard()
{

}


void DeckCard::setCode(QString code)
{
    this->code = code;

    if(!code.isEmpty())
    {
        cost = (*cardsJson)[code].value("cost").toInt();
        type = (*cardsJson)[code].value("type").toString();
        name = (*cardsJson)[code].value("name").toString();
        rarity = (*cardsJson)[code].value("rarity").toString();
    }
    else
    {
        cost = -1;
        type = "Minion";
        name = "unknown";
        rarity = "";
    }
}


QString DeckCard::tooltip()
{
    return "<html><img src=./HSCards/" + this->code + ".png/></html>";
}


void DeckCard::draw(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(canvas));
}

void DeckCard::drawGreyed(bool drawTotal, int cardHeight)
{
    QPixmap canvas = draw(drawTotal?this->total:this->remaining, false, BLACK, cardHeight);

    this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(
                            CARD_SIZE, QIcon::Disabled, QIcon::On)));
}

QPixmap DeckCard::draw(uint total, bool drawRarity, QColor nameColor, int cardHeight)
{
    QFont font;

    QPixmap canvas(CARD_CANVAS);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")               source = QRectF(63,18,100,25);
        else if(type==QString("Minion"))    source = QRectF(48,72,100,25);
        else                                source = QRectF(48,98,100,25);
        if(total == 1 && rarity != "Legendary") target = QRectF(113,6,100,25);
        else                                    target = QRectF(100,6,100,25);
        painter.drawPixmap(target, QPixmap(Utility::appPath() + "/HSCards/" + ((name=="unknown")?name:code) + ".png"), source);

        //Background and #cards
        if(drawRarity)                      painter.setPen(QPen(getRarityColor()));
        else if(nameColor!=BLACK)           painter.setPen(QPen(nameColor));
        else if(type==QString("Minion"))    painter.setPen(QPen(WHITE));
        else if (type==QString("Spell"))    painter.setPen(QPen(YELLOW));
        else                                painter.setPen(QPen(ORANGE));

        int maxNameLong;
        if(total == 1 && rarity != "Legendary")
        {
            maxNameLong = 174;
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard1.png"));
        }
        else
        {
            maxNameLong = 155;
            painter.drawPixmap(0,0,QPixmap(":Images/bgCard2.png"));

            if(total > 1)
            {
                font.setPointSize(16);
                painter.setFont(font);
                painter.drawText(QRectF(190,6,26,24), Qt::AlignCenter, QString::number(total));
            }
            else
            {
                font.setPointSize(20);
                painter.setFont(font);
                painter.drawText(QRectF(190,9,26,24), Qt::AlignCenter, "*");
            }
        }

        //Name and mana
        if(name == "unknown")
        {
            font.setPointSize(14);
            painter.setFont(font);
            painter.setPen(QPen(BLACK));
            painter.drawText(QRectF(34,7,154,23), Qt::AlignVCenter, "Unknown");
        }
        else
        {
            //Name
            int fontSize = 14;
            font.setPointSize(fontSize);
            font.setBold(true);

#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

            QFontMetrics fm(font);
            int textWide = fm.width(name);
            int textHigh = fm.height();
            while(textWide>maxNameLong)
            {
                fontSize--;
                font.setPointSize(fontSize);
                fm = QFontMetrics(font);
                textWide = fm.width(name);
                textHigh = fm.height();
            }

            painter.setFont(font);
            painter.setBrush(painter.pen().color());
            painter.setPen(QPen(BLACK));

            QPainterPath path;
            path.addText(34, 20 + textHigh/4, font, name);
            painter.drawPath(path);


            //Mana cost
            int manaSize = cost>9?6:cost;
            font.setPointSize(14+manaSize);
            painter.setFont(font);

            fm = QFontMetrics(font);
            textWide = fm.width(QString::number(cost));
            textHigh = fm.height();

            path = QPainterPath();
            path.addText(14 - textWide/2, 20 + textHigh/4, font, QString::number(cost));
            painter.drawPath(path);
        }
    painter.end();

    //Adapt to size
    canvas = canvas.scaled(CARD_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    return canvas;
}


QColor DeckCard::getRarityColor()
{
    if(rarity == "Free")               return WHITE;
    else if(rarity == "Common")        return WHITE;
    else if(rarity == "Rare")          return BLUE;
    else if(rarity == "Epic")          return VIOLET;
    else if(rarity == "Legendary")     return ORANGE;
    else                                return BLACK;
}


QString DeckCard::getCode()
{
    return code;
}


QString DeckCard::getType()
{
    return type;
}


QString DeckCard::getName()
{
    return name;
}


int DeckCard::getCost()
{
    return cost;
}


void DeckCard::setCardsJson(QMap<QString, QJsonObject> *cardsJson)
{
    DeckCard::cardsJson = cardsJson;
}
