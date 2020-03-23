#include <QtWidgets>

#include "QnetDeletePointDialog.h"

QnetDeletePointDialog::QnetDeletePointDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

