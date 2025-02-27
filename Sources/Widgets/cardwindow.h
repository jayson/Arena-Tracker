#ifndef CARDWINDOW_H
#define CARDWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>

#define HCARD 254
#define WCARD 182

class CardWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    CardWindow(QWidget *parent);
    ~CardWindow();

//Variables
private:
    QLabel *cardLabel;

//Metodos
public:


signals:

public slots:
    void LoadCard(QString code, QRect rectCard, int maxTop, int maxBottom);
};

#endif // CARDWINDOW_H
