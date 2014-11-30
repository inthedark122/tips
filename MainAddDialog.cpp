#include <QtGui>
#include "MainAddDialog.h"

MainAddDialog::MainAddDialog( QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Добавление записи"));
  setFixedSize(400, 200);

}
