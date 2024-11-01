#include "homepage.h"
#include "ui_homepage.h"
#include <QCoreApplication>
#include <QUrl>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

HomePage::HomePage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    //this->showFullScreen();
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->actionHalaman_1, &QAction::triggered, this, &HomePage::stackPage1);
    connect(ui->actionHalaman_2, &QAction::triggered, this, &HomePage::stackPage2);
    connect(ui->actionHalaman_3, &QAction::triggered, this, &HomePage::stackPage3);
    connect(ui->actionHalaman_4, &QAction::triggered, this, &HomePage::stackPage4);
    connect(ui->actionMap_3, &QAction::triggered, this, &HomePage::stackPage5);
    connect(ui->menuExit, &QAction::triggered, this, &HomePage::menuExit_triggered);
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::stackPage1() {
    ui->stackedWidget->setCurrentIndex(0);
}

void HomePage::stackPage2() {
    ui->stackedWidget->setCurrentIndex(1);
}

void HomePage::stackPage3() {
    // Buat QWebEngineView untuk menampilkan peta
    QWebEngineView *map3 = new QWebEngineView(this);
    QString mapPath = QCoreApplication::applicationDirPath() + "/map.html";

    // Periksa apakah file map.html ada
    QFile file(mapPath);
    if (!file.exists()) {
        qDebug() << "File map.html tidak ditemukan!";
        return;  // Keluar dari fungsi jika file tidak ada
    }

    // Mengizinkan semua sumber daya untuk QWebEngineView
    map3->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    // Set URL ke map.html
    map3->setUrl(QUrl::fromLocalFile(mapPath));
    map3->setMinimumSize(800, 600);

    // Hapus widget sebelumnya
    if (ui->verticalLayout->count() > 0) {
        QLayoutItem* item;
        while ((item = ui->verticalLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }

    ui->verticalLayout->addWidget(map3);
}



void HomePage::stackPage4() {
    // Pindahkan ke halaman yang diinginkan
    ui->stackedWidget->setCurrentIndex(3);

    // Pastikan widget sebelumnya dihapus agar tidak tumpang tindih
    if (ui->verticalLayout->count() > 0) {
        QLayoutItem* item;
        while ((item = ui->verticalLayout->takeAt(0)) != nullptr) {
            delete item->widget();  // Hapus widget sebelumnya dari layout
            delete item;  // Hapus item layout
        }
    }

    // Buat QWebEngineView baru untuk menampilkan peta
    QWebEngineView *view = new QWebEngineView(this);

    // Muat konten HTML langsung
    QString htmlContent = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Peta Stasiun Seismik</title>

            <!-- Leaflet.js CSS -->
            <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.3/dist/leaflet.css" />

            <!-- Leaflet.js JavaScript -->
            <script src="https://unpkg.com/leaflet@1.9.3/dist/leaflet.js"></script>

            <style>
                #map {
                    height: 600px;
                    width: 100%;
                }
                #input-container {
                    margin-bottom: 20px;
                }
                input {
                    margin-right: 10px;
                }
                #refresh {
                    margin-left: 10px;
                }
            </style>
        </head>
        <body>

            <h1>Peta Stasiun Seismik</h1>

            <div id="input-container">
                <label>Min Latitude: <input type="number" id="minlatitude" value="-45"></label>
                <label>Max Latitude: <input type="number" id="maxlatitude" value="-10"></label>
                <label>Min Longitude: <input type="number" id="minlongitude" value="122"></label>
                <label>Max Longitude: <input type="number" id="maxlongitude" value="155"></label>
                <button id="load-map">Load Map</button>
                <button id="refresh">Refresh</button>
            </div>

            <div id="map"></div>

            <script>
                // Inisialisasi peta dengan Leaflet.js
                var map = L.map('map').setView([-25, 140], 5);  // Peta dimulai di koordinat pusat Australia

                // Tambahkan tile layer untuk peta (OpenStreetMap)
                L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
                }).addTo(map);

                // Fungsi untuk memuat data stasiun seismik
                function loadStations() {
                    // Mengambil nilai dari input
                    var minLat = document.getElementById('minlatitude').value;
                    var maxLat = document.getElementById('maxlatitude').value;
                    var minLon = document.getElementById('minlongitude').value;
                    var maxLon = document.getElementById('maxlongitude').value;

                    // URL API
                    var api_url = "http://service.iris.edu/fdsnws/station/1/query?channel=BH*&maxlatitude=" + maxLat + "&minlatitude=" + minLat + "&minlongitude=" + minLon + "&maxlongitude=" + maxLon;

                    // Kosongkan layer lama
                    map.eachLayer(function(layer) {
                        if (!layer._url) { // Hanya hapus marker, bukan tile layer
                            map.removeLayer(layer);
                        }
                    });

                    // Mengambil data dari API menggunakan fetch
                    fetch(api_url)
                        .then(response => response.text())  // Mendapatkan data dalam bentuk teks (XML)
                        .then(data => {
                            // Menggunakan DOMParser untuk parse XML
                            var parser = new DOMParser();
                            var xmlDoc = parser.parseFromString(data, "application/xml");

                            // Mendapatkan semua elemen <Station>
                            var stations = xmlDoc.getElementsByTagName("Station");

                            // Iterasi setiap stasiun
                            for (var i = 0; i < stations.length; i++) {
                                var station = stations[i];

                                // Mendapatkan nama stasiun, latitude, dan longitude
                                var stationName = station.getElementsByTagName("StationName")[0]?.textContent || "Unknown Station";
                                var latitude = station.getElementsByTagName("Latitude")[0].textContent;
                                var longitude = station.getElementsByTagName("Longitude")[0].textContent;

                                // Membuat marker di peta untuk setiap stasiun
                                L.marker([latitude, longitude]).addTo(map)
                                    .bindPopup("<b>Station: </b>" + stationName);
                            }
                        })
                        .catch(error => console.log('Error:', error));
                }

                // Event listener untuk tombol "Load Map"
                document.getElementById('load-map').addEventListener('click', loadStations);

                // Event listener untuk tombol "Refresh"
                document.getElementById('refresh').addEventListener('click', loadStations);

            </script>

        </body>
        </html>
    )";

    // Tampilkan konten HTML di QWebEngineView
    view->setHtml(htmlContent);
    view->resize(800, 600);

    // Tambahkan QWebEngineView ke layout vertikal yang sudah ada
    ui->verticalLayout->addWidget(view);

    // Tampilkan QWebEngineView
    view->show();
}

void HomePage::stackPage5() {
   ui->stackedWidget->setCurrentIndex(4);
/*
    // Pastikan widget sebelumnya dihapus agar tidak tumpang tindih
    if (ui->verticalLayout->count() > 0) {
        QLayoutItem* item;
        while ((item = ui->verticalLayout->takeAt(0)) != nullptr) {
            delete item->widget();  // Hapus widget sebelumnya dari layout
            delete item;  // Hapus item layout
        }
    }

    // Buat QWebEngineView baru untuk menampilkan peta
    QWebEngineView *webView = new QWebEngineView(this);
    webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    // Panggil API untuk mengambil data stasiun
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://service.iris.edu/fdsnws/station/1/query?network=IU&format=text&level=station"));

    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QString data(response);

            // Ubah data menjadi format JavaScript untuk Leaflet
            QStringList lines = data.split("\n");
            QStringList stationData;
            for (int i = 1; i < lines.size(); ++i) {  // Mulai dari baris 1 untuk mengabaikan header
                QString line = lines.at(i);
                if (!line.isEmpty()) {
                    QStringList fields = line.split("|");
                    if (fields.size() > 4) {
                        QString latitude = fields.at(2).trimmed();
                        QString longitude = fields.at(3).trimmed();
                        QString stationName = fields.at(1).trimmed();

                        // Tambahkan bindTooltip untuk menampilkan nama stasiun saat kursor diarahkan
                        stationData.append(QString("L.marker([%1, %2]).addTo(map).bindPopup('%3').bindTooltip('%3');")
                                               .arg(latitude).arg(longitude).arg(stationName));
                    }
                }
            }

            // Buat HTML dengan Leaflet.js untuk menampilkan peta dengan marker stasiun
            QString htmlContent = R"(
                <!DOCTYPE html>
                <html lang="en">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>Peta Stasiun Seismik</title>

                    <!-- Leaflet.js CSS -->
                    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.3/dist/leaflet.css" />
                    <!-- Leaflet.js JavaScript -->
                    <script src="https://unpkg.com/leaflet@1.9.3/dist/leaflet.js"></script>

                    <style>
                        #map {
                            height: 600px;
                            width: 100%;
                        }
                    </style>
                </head>
                <body>
                    <h1>Peta Stasiun Seismik</h1>
                    <div id="map"></div>

                    <script>
                        // Inisialisasi peta
                        var map = L.map('map').setView([0, 0], 2);  // Pusatkan peta di koordinat global

                        // Tambahkan tile layer untuk peta
                        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
                        }).addTo(map);

                        // Tambahkan marker stasiun dengan tooltip untuk nama stasiun
                        )";

            htmlContent += stationData.join("\n");
            htmlContent += R"(
                    </script>
                </body>
                </html>
            )";

            // Set HTML ke QWebEngineView
            webView->setHtml(htmlContent);
        }
        reply->deleteLater();
    });

    manager->get(request);

    // Tambahkan QWebEngineView ke layout vertikal yang sudah ada
    ui->verticalLayout->addWidget(webView);

    // Tampilkan QWebEngineView
    webView->show();

*/

}


void HomePage::menuExit_triggered() {
    QApplication::quit();
}
