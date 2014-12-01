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
  showAll("");
  serviceShow("");
  connect(ui->actionNew_DB, SIGNAL(triggered()), this, SLOT(db_new()));
  connect(ui->actionDelete_DB, SIGNAL(triggered()), this, SLOT(db_delete()));

  //Управление компаниями
  connect(ui->main_add, SIGNAL(clicked()), this, SLOT(add_to_db()));
  connect(ui->main_delete, SIGNAL(clicked()), this, SLOT(main_delete()));
  connect(ui->main_find, SIGNAL(clicked()), this, SLOT(main_find()));

  // Service
  connect(ui->service_add, SIGNAL(clicked()), this, SLOT(service_add()));
  connect(ui->service_delete, SIGNAL(clicked()), this, SLOT(service_delete()));
}

MainWindow::~MainWindow(){
  delete ui;
}

bool MainWindow::openDB() {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbName);
  return db.open();
}

void MainWindow::db_new() {
  if (db.isOpen()) {
    QMessageBox::warning(this, ("Уведомление"), "База уже открыта");
    return;
  }
  openDB();
  createTabel();
  showAll("");
}

void MainWindow::db_delete() {
  db.close();
  QFile::remove(dbName);
  QMessageBox::warning(this, ("Уведомление"), "База удалена");
}

void MainWindow::createTabel() {
  QSqlQuery a_query;
  QString table_company = "CREATE TABLE company (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, name VARCHAR(255), site VARCHAR(255));";
  a_query.exec(table_company);

  QString table_service = "CREATE TABLE service (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, name VARCHAR(255), desc VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_service);

  QString table_price = "CREATE TABLE price (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, service_id integer, price VARCHAR(255), valute VARCHAR(255), FOREIGN KEY (sevice_id) REFERENCES service(id));";
  a_query.exec(table_price);

  QString table_adres = "CREATE TABLE adres (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, adres VARCHAR(255), city VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_adres);

  QString table_pay_system = "CREATE TABLE pay_system (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, name VARCHAR(255), desc VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_pay_system);

}

void MainWindow::showAll(QString query = "") {
  while (ui->listDB->rowCount() > 0){ui->listDB->removeRow(0);}
  while (ui->service_company_id->count() > 0){ui->service_company_id->removeItem(0);}
  QSqlQuery a_query;
  if (query == "")
    a_query.exec("SELECT * FROM company");
  else
    a_query.exec(query);
  QSqlRecord rec = a_query.record();
  int number_column = 0, name_column = 1, site_column = 2;
  QString ids = "";
  while (a_query.next()) {
    int index = ui->listDB->rowCount();
    ui->listDB->insertRow(index);

    QString id_str = a_query.value(rec.indexOf("id")).toString();
    ids += id_str +",";
    QTableWidgetItem *id = new QTableWidgetItem(id_str);
    ui->listDB->setItem(index, number_column, id);

    QString name_str = a_query.value(rec.indexOf("name")).toString();
    QTableWidgetItem *name = new QTableWidgetItem(name_str);
    ui->listDB->setItem(index, name_column, name);

    QTableWidgetItem *site = new QTableWidgetItem(a_query.value(rec.indexOf("site")).toString());
    ui->listDB->setItem(index, site_column, site);

    ui->service_company_id->addItem(name_str, id_str);
  }
  ids.remove(ids.length()-1,1);
  QString str_service_find = "select * from service where company_id IN (%1)";
  QString str_service = str_service_find.arg(ids);
  serviceShow(str_service);
}


// Управление компаниями
void MainWindow::add_to_db() {
    QString name = ui->main_name->text();
    QString site = ui->main_site->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO company(name, site) VALUES ('%1', '%2');";
    QString str = str_insert.arg(name).arg(site);
    bool b = a_query.exec(str);
    if (!b) { qDebug() << "Кажется данные не вставляются, проверьте дверь, может она закрыта?"; }
    showAll();
}

void MainWindow::main_delete() {
  int row = ui->listDB->currentRow();
  if (row == -1) return;
  int id = ui->listDB->item(row, 0)->text().toInt();
  QSqlQuery a_query;
  QString str_delete = "DELETE from company where id=%1;";
  QString str = str_delete.arg(id);
  bool b = a_query.exec(str);
  if (!b) qDebug() << "Не могу удалить";
  showAll();
}

void MainWindow::main_find() {
  QString name = ui->main_name->text();
  QString site = ui->main_site->text();
  if (name != "" || site != "") {
    QString str_main_find = "select * from company where name LIKE '%%1%' AND site LIKE '%%2%' ";
    QString str_main = str_main_find.arg(name).arg(site);
    showAll(str_main);
  } else {
    showAll("");
  }
}

// Сервисы
void MainWindow::serviceShow(QString query) {
    while (ui->serviceTable->rowCount() > 0){ui->serviceTable->removeRow(0);}
    QSqlQuery a_query;
    if (query == "")
      a_query.exec("SELECT * FROM service");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, company_id_column = 1, name_column = 2, desc_column = 3;
    while (a_query.next()) {
      int index = ui->serviceTable->rowCount();
      ui->serviceTable->insertRow(index);
      QTableWidgetItem *id = new QTableWidgetItem(a_query.value(rec.indexOf("id")).toString());
      ui->serviceTable->setItem(index, id_column, id);

      QTableWidgetItem *company_id = new QTableWidgetItem(a_query.value(rec.indexOf("company_id")).toString());
      ui->serviceTable->setItem(index, company_id_column, company_id);

      QTableWidgetItem *name = new QTableWidgetItem(a_query.value(rec.indexOf("name")).toString());
      ui->serviceTable->setItem(index, name_column, name);

      QTableWidgetItem *desc = new QTableWidgetItem(a_query.value(rec.indexOf("desc")).toString());
      ui->serviceTable->setItem(index, desc_column, desc);
    }
}

void MainWindow::service_add() {
    int company_id_index = ui->service_company_id->currentIndex();
    int company_id = ui->service_company_id->itemData(company_id_index).toInt();
    QString name = ui->service_name->text();
    QString desc = ui->service_desc->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO service(company_id, name, desc) VALUES (%1, '%2', '%3');";
    QString str = str_insert.arg(company_id).arg(name).arg(desc);
    bool b = a_query.exec(str);
    if (!b) { qDebug() << "Кажется данные не вставляются, проверьте дверь, может она закрыта?"; }
    serviceShow("");
}

void MainWindow::service_delete() {
  int row = ui->serviceTable->currentRow();
  if (row == -1) return;
  int id = ui->serviceTable->item(row, 0)->text().toInt();
  QSqlQuery a_query;
  QString str_delete = "DELETE from service where id=%1;";
  QString str = str_delete.arg(id);
  bool b = a_query.exec(str);
  if (!b) qDebug() << "Не могу удалить";
  serviceShow("");
}
