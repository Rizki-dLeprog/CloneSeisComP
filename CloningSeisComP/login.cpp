#include "login.h"
#include "./ui_login.h"
#include "homepage.h"


login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    connectDB();
}

login::~login()
{
    delete ui;
}

void login::connectDB() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Latihan_QT_QML/CloningSeisComP/database/users.db");

    // Optional: karena hanya memeriksa driver pada installation
    // Directori:\Qt\6.7.2\mingw_64\plugins\sqldrivers
    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Available drivers:" << drivers;
    if (!drivers.contains("QSQLITE")) {
        QMessageBox::critical(this, "Driver Error", "SQLite driver tidak ditemukan!");
        return;
    }

    if (!db.open()) {
        QMessageBox::critical(this, "Error Dialog", "Cek Directori Penyimpanan! " +
                                                        db.lastError().text());
        return;
    } else {
        qDebug() << "Database Terhubung!";
    }
}

void login::on_pushButtonLogin_clicked(){
    QString user = ui->lineEditUser->text();
    QString pass = ui->lineEditPass->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", user);
    query.bindValue(":password", pass);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "cek query database!");
        qDebug() << query.lastError().text();
        return;
    }
    if (query.next()) {
        QMessageBox::information(this, "Information Dialog", "Welcome To The Jungle " + user);

        HomePage *hp = new HomePage();
        hp->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Warning Dialog", "Check Username & Password");
        return;
    }
}




void login::on_pushButtonCancel_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Dialog", "Apa Kamu Yakin?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
