#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>
#include <QWebEngineView>

namespace Ui {
class HomePage;
}

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

private slots:
    void stackPage1();
    void stackPage2();
    void stackPage3();
    void stackPage4();
    void stackPage5();
    void menuExit_triggered();

private:
    Ui::HomePage *ui;
    QWebEngineView *map; // Peta untuk stackPage3
    QWebEngineView *mapseismik_indonesia; // Peta untuk stackPage4
};

#endif // HOMEPAGE_H
