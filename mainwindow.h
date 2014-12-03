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
  void adresShow(QString);
  void pay_serviceShow(QString);

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
  // price
  void price_add();
  void price_delete();
  // adres
  void adres_add();
  void adres_delete();
  // pay_system
  void pay_service_add();
  void pay_service_delete();

private:
  QSqlDatabase db;
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
