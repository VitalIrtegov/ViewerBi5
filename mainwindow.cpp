#include "mainwindow.h"
//#include "lzmadecoder.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QtEndian>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /*LzmaDecoder decoder;
    decoder.inspectBi5File();
    QCoreApplication::exit(0);*/

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);

    // Создаем горизонтальный слой для кнопок
    auto *buttonLayout = new QHBoxLayout();

    auto *btnOpen = new QPushButton("Открыть", this);
    auto *btnSave = new QPushButton("Сохранить", this);

    // Ограничиваем ширину кнопок, чтобы они не растягивались
    btnOpen->setFixedWidth(100);
    btnSave->setFixedWidth(100);

    // Добавляем кнопки в горизонтальный слой
    buttonLayout->addWidget(btnOpen);
    buttonLayout->addWidget(btnSave);
    buttonLayout->addStretch(); // Этот "распорка" прижмет кнопки влево

    listWidget = new QListWidget(this);
    listWidget->setFont(QFont("Courier New", 10));

    // Добавляем слои в главный контейнер
    mainLayout->addLayout(buttonLayout); // Сначала кнопки
    mainLayout->addWidget(listWidget);    // Потом список

    setCentralWidget(centralWidget);
    resize(800, 500);

    decoder = new LzmaDecoder();

    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenClicked);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
}

MainWindow::~MainWindow() { }

void MainWindow::onOpenClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Открыть Bi5", "", "Dukascopy (*.bi5)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray compressedData = file.readAll();
    file.close();

    // Получаем данные из нашего "черного ящика"
    QVector<Tick> ticks = decoder->decodeBi5(compressedData);

    listWidget->clear();

    // 1. Добавляем строку заголовка
    QString header = QString("%1 | %2 | %3 | %4 | %5 | %6")
                         .arg("#", 5)
                         .arg("Time (ms)", 10)
                         .arg("AskPrice", 10)
                         .arg("BidPrice", 10)
                         .arg("AskVol", 8)
                         .arg("BidVol", 8);

    listWidget->addItem(header);
    listWidget->addItem(QString("-").repeated(header.length())); // Разделитель

    // 2. Выводим ВСЕ тики без исключения
    for (int i = 0; i < ticks.size(); ++i) {
        const Tick &t = ticks[i];

        // i + 1 — это номер строки
        // arg(..., ширина, система_счисления, символ_заполнитель)
        QString row = QString("%1 | %2 | %3 | %4 | %5 | %6")
                          .arg(i + 1, 5)                          // Номер строки
                          .arg(t.timeMS, 10, 10, QChar('0'))      // Время
                          .arg(t.ask, 10, 'f', 5)                 // Ask
                          .arg(t.bid, 10, 'f', 5)                 // Bid
                          .arg((double)t.askVol, 8, 'f', 2)       // Ask Volume
                          .arg((double)t.bidVol, 8, 'f', 2);      // Bid Volume

        listWidget->addItem(row);
    }
}

void MainWindow::onSaveClicked() {
    if (listWidget->count() <= 2) {
        QMessageBox::warning(this, "Внимание", "Сохранять нечего! Нужно сперва открыть что нибудь");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Сохранить результат", "", "Text Files (*.txt)");

    if (savePath.isEmpty()) return;

    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Проходим по всем строкам listWidget и записываем их в файл
        for (int i = 0; i < listWidget->count(); ++i) {
            out << listWidget->item(i)->text() << "\n";
        }

        file.close();
        //qDebug() << "Файл успешно сохранен:" << savePath;
    }
}
