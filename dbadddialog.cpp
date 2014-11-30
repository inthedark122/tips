#include "dbadddialog.h"
#include "ui_dbadddialog.h"

DBAddDialog::DBAddDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DBAddDialog)
{
  ui->setupUi(this);
  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

DBAddDialog::~DBAddDialog()
{
  delete ui;
}

void DBAddDialog::accept(*parent) {
  parent->accept();
}
