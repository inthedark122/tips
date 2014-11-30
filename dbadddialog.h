#ifndef DBADDDIALOG_H
#define DBADDDIALOG_H

#include <QDialog>

namespace Ui {
  class DBAddDialog;
}

class DBAddDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DBAddDialog(QWidget *parent = 0);
  ~DBAddDialog();

public slots:
  void accept();

private:
  Ui::DBAddDialog *ui;
};

#endif // DBADDDIALOG_H
