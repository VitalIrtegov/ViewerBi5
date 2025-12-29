#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lzmadecoder.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onOpenClicked();
    void onSaveClicked();
private:
    QListWidget *listWidget;
    LzmaDecoder *decoder;
};
#endif // MAINWINDOW_H
