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

  void showAll(QString);
  void serviceShow(QString);

  QString dbName = "tmp";

public slots:
  void add_to_db();
  void db_new();
  void db_delete();
  void main_delete();
  void main_find();

  // service
  void service_add();
  void service_delete();

private:
  QSqlDatabase db;
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
