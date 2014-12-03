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

  // price
  connect(ui->price_add, SIGNAL(clicked()), this, SLOT(price_add()));
  connect(ui->price_delete, SIGNAL(clicked()), this, SLOT(price_delete()));

  // adres
  connect(ui->adres_add, SIGNAL(clicked()), this, SLOT(adres_add()));
  connect(ui->adres_delete, SIGNAL(clicked()), this, SLOT(adres_delete()));

  // pay_service
  connect(ui->pay_service_add, SIGNAL(clicked()), this, SLOT(pay_service_add()));
  connect(ui->pay_service_delete, SIGNAL(clicked()), this, SLOT(pay_service_delete()));


  //pay_system
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
    //QMessageBox::warning(this, ("Уведомление"), "База закрыта.");
    ui->db_status->setText("База закрыта.");
  } else {
    //QMessageBox::warning(this, ("Ошибка"), "База не открыта.");
    ui->db_status->setText("База закрыта.");
  }
}

void MainWindow::db_add() {
  if (db.isOpen()) {
    //QMessageBox::warning(this, ("Ошибка"), "Закройте базу перед созданием.");
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
  //QMessageBox::warning(this, ("Уведомление"), "База удалена");
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

  QString table_adres = "CREATE TABLE adres (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, adres VARCHAR(255), city VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_adres);

  QString table_pay_system = "CREATE TABLE pay_system (id integer PRIMARY KEY AUTOINCREMENT NOT NULL, company_id integer, name VARCHAR(255), desc VARCHAR(255), FOREIGN KEY (company_id) REFERENCES company(id));";
  a_query.exec(table_pay_system);
}

void MainWindow::showAll() {
  mainShow("");
  serviceShow("");
  priceShow("");
  adresShow("");
  pay_serviceShow("");
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
    ui->adres_company_id->addItem(name_str, id_str);
    ui->pay_service_company_id->addItem(name_str, id_str);
  }
  ids.remove(ids.length()-1,1);
  QString str_service_find = "select * from service where company_id IN (%1);";
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

      QString id_str = a_query.value(rec.indexOf("id")).toString();
      QTableWidgetItem *id = new QTableWidgetItem(id_str);
      ui->serviceTable->setItem(index, id_column, id);

      QTableWidgetItem *company_id = new QTableWidgetItem(a_query.value(rec.indexOf("company_id")).toString());
      ui->serviceTable->setItem(index, company_id_column, company_id);

      QString name_str = a_query.value(rec.indexOf("name")).toString();
      QTableWidgetItem *name = new QTableWidgetItem(name_str);
      ui->serviceTable->setItem(index, name_column, name);

      QTableWidgetItem *desc = new QTableWidgetItem(a_query.value(rec.indexOf("desc")).toString());
      ui->serviceTable->setItem(index, desc_column, desc);

       ui->price_service_id->addItem(name_str, id_str);
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
      a_query.exec("select * from price;");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, service_id_column = 1, price_column = 2, valuta_column = 3;
    while (a_query.next()) {

      int index = ui->priceTable->rowCount();
      ui->priceTable->insertRow(index);
      QTableWidgetItem *id = new QTableWidgetItem(a_query.value(rec.indexOf("id")).toString());
      ui->priceTable->setItem(index, id_column, id);

      QTableWidgetItem *service_id = new QTableWidgetItem(a_query.value(rec.indexOf("service_id")).toString());
      ui->priceTable->setItem(index, service_id_column, service_id);

      QTableWidgetItem *price = new QTableWidgetItem(a_query.value(rec.indexOf("price")).toString());
      ui->priceTable->setItem(index, price_column, price);

      QTableWidgetItem *valuta = new QTableWidgetItem(a_query.value(rec.indexOf("valuta")).toString());
      ui->priceTable->setItem(index, valuta_column, valuta);
    }
}

void MainWindow::price_add() {
    int service_id_index = ui->service_company_id->currentIndex();
    int service_id = ui->service_company_id->itemData(service_id_index).toInt();
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

// Адреса
void MainWindow::adresShow(QString query) {
    while (ui->adresTable->rowCount() > 0){ui->adresTable->removeRow(0);}
    QSqlQuery a_query;
    if (query == "")
      a_query.exec("SELECT * FROM adres");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, company_id_column = 1, adres_column = 2, city_column = 3;
    while (a_query.next()) {
      int index = ui->adresTable->rowCount();
      ui->adresTable->insertRow(index);
      QTableWidgetItem *id = new QTableWidgetItem(a_query.value(rec.indexOf("id")).toString());
      ui->adresTable->setItem(index, id_column, id);

      QTableWidgetItem *company_id = new QTableWidgetItem(a_query.value(rec.indexOf("company_id")).toString());
      ui->adresTable->setItem(index, company_id_column, company_id);

      QTableWidgetItem *adres = new QTableWidgetItem(a_query.value(rec.indexOf("adres")).toString());
      ui->adresTable->setItem(index, adres_column, adres);

      QTableWidgetItem *city = new QTableWidgetItem(a_query.value(rec.indexOf("city")).toString());
      ui->adresTable->setItem(index, city_column, city);
    }
}
void MainWindow::adres_add() {
    int company_id_index = ui->adres_company_id->currentIndex();
    int company_id = ui->adres_company_id->itemData(company_id_index).toInt();
    QString adres = ui->adres_adres->text();
    QString city = ui->adres_city->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO adres(company_id, adres, city) VALUES (%1, '%2', '%3');";
    QString str = str_insert.arg(company_id).arg(adres).arg(city);
    bool b = a_query.exec(str);
    if (!b) QMessageBox::warning(this, ("Ошибка"), "Кажется данные не вставляются, проверьте дверь, может она закрыта?");
    adresShow("");
}
void MainWindow::adres_delete() {
  int row = ui->adresTable->currentRow();
  if (row == -1) return;
  int id = ui->adresTable->item(row, 0)->text().toInt();
  QSqlQuery a_query;
  QString str_delete = "DELETE from adres where id=%1;";
  QString str = str_delete.arg(id);
  bool b = a_query.exec(str);
  if (!b) QMessageBox::warning(this, ("Ошибка"), "Не могу удалить");
  adresShow("");
}

// Платежная система
void MainWindow::pay_serviceShow(QString query) {
    while (ui->pay_serviceTable->rowCount() > 0){ui->pay_serviceTable->removeRow(0);}
    QSqlQuery a_query;
    if (query == "")
      a_query.exec("SELECT * FROM pay_system");
    else
      a_query.exec(query);
    QSqlRecord rec = a_query.record();
    int id_column = 0, company_id_column = 1, name_column = 2, desc_column = 3;
    while (a_query.next()) {
      int index = ui->pay_serviceTable->rowCount();
      ui->pay_serviceTable->insertRow(index);
      QTableWidgetItem *id = new QTableWidgetItem(a_query.value(rec.indexOf("id")).toString());
      ui->pay_serviceTable->setItem(index, id_column, id);

      QTableWidgetItem *company_id = new QTableWidgetItem(a_query.value(rec.indexOf("company_id")).toString());
      ui->pay_serviceTable->setItem(index, company_id_column, company_id);

      QTableWidgetItem *name = new QTableWidgetItem(a_query.value(rec.indexOf("name")).toString());
      ui->pay_serviceTable->setItem(index, name_column, name);

      QTableWidgetItem *desc = new QTableWidgetItem(a_query.value(rec.indexOf("desc")).toString());
      ui->pay_serviceTable->setItem(index, desc_column, desc);
    }
}
void MainWindow::pay_service_add() {
    int company_id_index = ui->pay_service_company_id->currentIndex();
    int company_id = ui->pay_service_company_id->itemData(company_id_index).toInt();
    QString name = ui->pay_service_name->text();
    QString desc = ui->pay_service_desc->text();
    QSqlQuery a_query;
    QString str_insert = "INSERT INTO pay_system(company_id, name, desc) VALUES (%1, '%2', '%3');";
    QString str = str_insert.arg(company_id).arg(name).arg(desc);
    bool b = a_query.exec(str);
    if (!b) QMessageBox::warning(this, ("Ошибка"), "Кажется данные не вставляются.");
    pay_serviceShow("");
}
void MainWindow::pay_service_delete() {
  int row = ui->pay_serviceTable->currentRow();
  if (row == -1) return;
  int id = ui->pay_serviceTable->item(row, 0)->text().toInt();
  QSqlQuery a_query;
  QString str_delete = "DELETE from pay_system where id=%1;";
  QString str = str_delete.arg(id);
  bool b = a_query.exec(str);
  if (!b) QMessageBox::warning(this, ("Ошибка"), "Не могу удалить");
  pay_serviceShow("");
}
