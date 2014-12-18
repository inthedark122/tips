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
  connect(ui->db_open, SIGNAL(clicked()), this, SLOT(db_open()));
  connect(ui->db_close, SIGNAL(clicked()), this, SLOT(db_close()));
  connect(ui->db_add, SIGNAL(clicked()), this, SLOT(db_add()));
  connect(ui->db_delete, SIGNAL(clicked()), this, SLOT(db_delete()));

  //Управление компаниями
  connect(ui->main_add, SIGNAL(clicked()), this, SLOT(main_add()));
  connect(ui->main_delete, SIGNAL(clicked()), this, SLOT(main_delete()));
  connect(ui->main_find, SIGNAL(clicked()), this, SLOT(main_find()));

  // Service
  connect(ui->service_add, SIGNAL(clicked()), this, SLOT(service_add()));
  connect(ui->service_delete, SIGNAL(clicked()), this, SLOT(service_delete()));
  connect(ui->service_find, SIGNAL(clicked()), this, SLOT(service_find()));

  // price
  connect(ui->price_add, SIGNAL(clicked()), this, SLOT(price_add()));
  connect(ui->price_delete, SIGNAL(clicked()), this, SLOT(price_delete()));
}

MainWindow::~MainWindow(){
  delete ui;
}

void MainWindow::db_open() {
  dbName = ui->db_name->text()+".sqlite";
  if (!QFile::exists(dbName)) {
    ui->db_status->setText("Не создана");
    return;
  }
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbName);
  if (db.open()) {
    ui->db_status->setText("База открыта");
    showAll();
  } else {
    ui->db_status->setText("уже открыта");
    showAll();
  }
}

void MainWindow::db_close() {
  if (db.open()) {
    db.close();
    ui->db_status->setText("База закрыта.");
  } else {
    ui->db_status->setText("База закрыта.");
  }
}

void MainWindow::db_add() {
  if (db.isOpen()) {
    ui->db_status->setText("База уже открыта.");
    return;
  }
  dbName = ui->db_name->text()+".sqlite";
  if (QFile::exists(dbName)) {
    QMessageBox::warning(this, ("Ошибка"), "База уже создана и ее можно открыть.");
  } else {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    db.open();
    createTabel();
    ui->db_status->setText("База создана и открыта.");
    showAll();
  }
}


void MainWindow::db_delete() {
  if (db.open()) { db.close(); }
  QFile::remove(dbName);
  ui->db_status->setText("удалена");
}
void MainWindow::createTabel() {
  QSqlQuery a_query;
  QString table_company = "CREATE TABLE company (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, name VARCHAR(255), site VARCHAR(255));";
  a_query.exec(table_company);

  QString table_service = "CREATE TABLE service (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, name VARCHAR(255), desc VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_service);

  QString table_price = "CREATE TABLE price (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, service_id integer, price VARCHAR(255), valuta VARCHAR(255), FOREIGN KEY (service_id) REFERENCES service(id));";
  a_query.exec(table_price);
}

void MainWindow::showAll() {
  mainShow("");
  serviceShow("");
  priceShow("");
}

// Управление компаниями
void MainWindow::mainShow(QString query = "") {
  while (ui->listDB->rowCount() > 0){ui->listDB->removeRow(0);}
  while (ui->service_company_id->count() > 0){ui->service_company_id->removeItem(0);}
  QSqlQuery a_query;
  if (query == "")
    a_query.exec("SELECT * FROM company;");
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
  QString str_service_find = "SELECT s.id, s.name, s.desc, c.name as company_name FROM service as s JOIN company AS c ON c.id = s.company_id where company_id IN (%1);";
  QString str_service = str_service_find.arg(ids);
  serviceShow(str_service);
}

void MainWindow::main_add() {
    QString name = ui->main_name->text();
    QString site = ui->main_site->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO company(name, site) VALUES ('%1', '%2');";
    QString str = str_insert.arg(name).arg(site);
    bool b = a_query.exec(str);
    if (!b) { QMessageBox::warning(this, ("Ошибка"), "Кажется данные не вставляются."); }
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
  if (!b) QMessageBox::warning(this, ("Ошибка"), "Не могу удалить");
  showAll();
}
void MainWindow::main_find() {
  QString name = ui->main_name->text();
  QString site = ui->main_site->text();
  if (name != "" || site != "") {
    QString str_main_find = "select * from company where name LIKE '%%1%' AND site LIKE '%%2%' ";
    QString str_main = str_main_find.arg(name).arg(site);
    mainShow(str_main);
  } else {
    mainShow("");
  }
}

// Сервисы
void MainWindow::serviceShow(QString query) {
    while (ui->serviceTable->rowCount() > 0){ui->serviceTable->removeRow(0);}
    while (ui->price_service_id->count() > 0){ui->price_service_id->removeItem(0);}
    QSqlQuery a_query;
    if (query == "")
      a_query.exec("SELECT s.id, s.name, s.desc, c.name as company_name FROM service as s JOIN company AS c ON c.id = s.company_id;");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, company_id_column = 1, name_column = 2, desc_column = 3;
    QString ids = "";
    while (a_query.next()) {
      int index = ui->serviceTable->rowCount();
      ui->serviceTable->insertRow(index);
      qDebug() << index;
      QString id_str = a_query.value(rec.indexOf("id")).toString();
      QTableWidgetItem *id = new QTableWidgetItem(id_str);
      ui->serviceTable->setItem(index, id_column, id);
      ids += id_str +",";

      QTableWidgetItem *company_id = new QTableWidgetItem(a_query.value(rec.indexOf("company_name")).toString());
      ui->serviceTable->setItem(index, company_id_column, company_id);

      QString name_str = a_query.value(rec.indexOf("name")).toString();
      QTableWidgetItem *name = new QTableWidgetItem(name_str);
      ui->serviceTable->setItem(index, name_column, name);

      QTableWidgetItem *desc = new QTableWidgetItem(a_query.value(rec.indexOf("desc")).toString());
      ui->serviceTable->setItem(index, desc_column, desc);

      ui->price_service_id->addItem(name_str, id_str);
    }
    ids.remove(ids.length()-1,1);
    QString str_price_find = "select p.id, p.price, p.valuta, s.name, p.service_id from price as p JOIN service AS s ON s.id = p.service_id where p.service_id IN (%1);";
    QString str_price = str_price_find.arg(ids);
    priceShow(str_price);
}

void MainWindow::service_find() {
    int row = ui->serviceTable->currentRow();
    if (row == -1) return;
    int id = ui->serviceTable->item(row, 0)->text().toInt();
    QString str_find_price = "select p.id, p.price, p.valuta, s.name, p.service_id from price as p JOIN service AS s ON s.id = p.service_id WHERE p.service_id = %1;";
    QString find_price = str_find_price.arg(id);
    qDebug() << find_price;
    priceShow(find_price);
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
    if (!b) QMessageBox::warning(this, ("Ошибка"), "Кажется данные не вставляются, проверьте дверь, может она закрыта?");
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
  if (!b) QMessageBox::warning(this, ("Ошибка"), "Не могу удалить");
  serviceShow("");
}

// Цены
void MainWindow::priceShow(QString query) {
    while (ui->priceTable->rowCount() > 0){ui->priceTable->removeRow(0);}
    QSqlQuery a_query;
    if (query == "")
      a_query.exec("select p.id, p.price, p.valuta, s.name, p.service_id from price as p JOIN service AS s ON s.id = p.service_id;");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, service_id_column = 1, price_column = 2, valuta_column = 3;
    while (a_query.next()) {

      int index = ui->priceTable->rowCount();
      ui->priceTable->insertRow(index);
      QTableWidgetItem *id = new QTableWidgetItem(a_query.value(rec.indexOf("id")).toString());
      ui->priceTable->setItem(index, id_column, id);

      QTableWidgetItem *service_id = new QTableWidgetItem(a_query.value(rec.indexOf("name")).toString());
      ui->priceTable->setItem(index, service_id_column, service_id);

      QTableWidgetItem *price = new QTableWidgetItem(a_query.value(rec.indexOf("price")).toString());
      ui->priceTable->setItem(index, price_column, price);

      QTableWidgetItem *valuta = new QTableWidgetItem(a_query.value(rec.indexOf("valuta")).toString());
      ui->priceTable->setItem(index, valuta_column, valuta);
    }
}

void MainWindow::price_add() {
    int service_id_index = ui->price_service_id->currentIndex();
    int service_id = ui->price_service_id->itemData(service_id_index).toInt();
    QString price = ui->price_price->text();
    QString valuta = ui->price_valuta->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO price(service_id, price, valuta) VALUES (%1, '%2', '%3');";
    QString str = str_insert.arg(service_id).arg(price).arg(valuta);
    bool b = a_query.exec(str);
    if (!b) QMessageBox::warning(this, ("Ошибка"), "Кажется данные не вставляются.");
    priceShow("");
}

void MainWindow::price_delete() {
  int row = ui->priceTable->currentRow();
  if (row == -1) return;
  int id = ui->priceTable->item(row, 0)->text().toInt();
  QSqlQuery a_query;
  QString str_delete = "DELETE from price where id=%1;";
  QString str = str_delete.arg(id);
  bool b = a_query.exec(str);
  if (!b) QMessageBox::warning(this, ("Ошибка"), "Не могу удалить");
  priceShow("");
}

