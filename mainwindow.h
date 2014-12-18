#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QFile>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  bool openDB();
  void createTabel();

  void showAll();
  void mainShow(QString);
  void serviceShow(QString);
  void priceShow(QString);
  QString dbName;

public slots:
  void db_open();
  void db_close();
  void db_add();
  void db_delete();

  //main
  void main_add();
  void main_delete();
  void main_find();
  // service
  void service_add();
  void service_delete();
  void service_find();
  // price
  void price_add();
  void price_delete();

private:
  QSqlDatabase db;
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
