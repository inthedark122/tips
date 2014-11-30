#include "mainwindow.h"
#include <QtSql>
#include <QDebug>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QDialog>
#include "ui_mainwindow.h"
#include "dbadddialog.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  openDB();
  insertRow();
  showAll();
  connect(ui->actionNew_DB, SIGNAL(triggered()), this, SLOT(db_new()));
  connect(ui->actionDelete_DB, SIGNAL(triggered()), this, SLOT(db_delete()));
  connect(ui->addToDb, SIGNAL(clicked()), this, SLOT(add_to_db()));
}

MainWindow::~MainWindow()
{
  delete ui;
}



bool MainWindow::openDB() {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbName);
  return db.open();
}

void MainWindow::insertRow() {
  QSqlQuery a_query;
  QString str_insert = "INSERT INTO my_table(number, address, age) VALUES (%1, '%2', %3);";
  QString str = str_insert.arg("15")
                  .arg("hello world str.")
                  .arg("37");
  bool b = a_query.exec(str);
  if (!b) {
    qDebug() << "Кажется данные не вставляются, проверьте дверь, может она закрыта?";
  }
}

void MainWindow::db_new() {
  if (db.isOpen()) {
    QMessageBox::warning(this, ("Уведомление"), "База уже создана");
    return;
  }
  openDB();
  createTabel();
  insertRow();
  showAll();
}

void MainWindow::db_delete() {
  db.close();
  QFile::remove(dbName);
  for(int i =0; i < ui->listDB->rowCount(); i++) {
    ui->listDB->removeRow(i);
  }
  QMessageBox::warning(this, ("Уведомление"), "База удалена");
}

void MainWindow::createTabel() {
  QSqlQuery a_query;
  // DDL query
  QString str = "CREATE TABLE my_table ("
           "number integer PRIMARY KEY NOT NULL, "
           "address VARCHAR(255), "
           "age integer"
           ");";
  bool b = a_query.exec(str);
  if (!b) {
    qDebug() << "Вроде не удается создать таблицу, провертье карманы!";
  }
}

void MainWindow::showAll() {
  ui->listDB->clearContents();
  QSqlQuery a_query;
  a_query.exec("SELECT * FROM my_table");
  QSqlRecord rec = a_query.record();
  int number_column = 0, age_column = 2, address_column = 1;
  while (a_query.next()) {
    int index = ui->listDB->rowCount();
    ui->listDB->insertRow(index);
    //number = a_query.value(rec.indexOf("number")).toInt();
    QTableWidgetItem *number = new QTableWidgetItem(a_query.value(rec.indexOf("number")).toString());
    ui->listDB->setItem(index, number_column, number);

    QTableWidgetItem *age = new QTableWidgetItem(a_query.value(rec.indexOf("age")).toString());
    ui->listDB->setItem(index, age_column, age);

    QTableWidgetItem *address = new QTableWidgetItem(a_query.value(rec.indexOf("address")).toString());
    ui->listDB->setItem(index, address_column, address);
  }
}


// SIGNAL
void MainWindow::add_to_db() {
  DBAddDialog *add =  new DBAddDialog(this);
  add->exec();
}

void MainWindow::accept() {
  QMessageBox::warning(this, ("Уведомление"), "Добавили");
}
