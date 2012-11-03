

#include "LoginDialog.h"
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

void LoginDialog::process_reject()
{
  this->reject();
}
void LoginDialog::process_accept()
{
  this->m_isGood = true;
  this->accept();
}

LoginDialog::LoginDialog(QWidget * parent) :
  QDialog(parent),
  lineEdit_server(new QLineEdit(this)),
  lineEdit_appname(new QLineEdit(this)),
  lineEdit_email(new QLineEdit(this)),
  lineEdit_apikey(new QLineEdit(this)),
  lineEdit_descriptor(new QLineEdit(this)),

  m_isGood(false)
{
  QVBoxLayout * verticalLayout = new QVBoxLayout(this);
  QGridLayout * gridLayout = new QGridLayout();
  QLabel * label_server   = new QLabel("Server URL:", this);
  QLabel * label_appname  = new QLabel("API Application Name:", this);
  QLabel * label_email    = new QLabel("E-mail:", this);
  QLabel * label_apikey   = new QLabel("API Key:", this);
  QLabel * label_descriptor   = new QLabel("File Descriptor:", this);
  QDialogButtonBox * buttonBox = new QDialogButtonBox();

  this->resize(529, 200);
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);

  label_server->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label_appname->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label_email->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label_apikey->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

  gridLayout->addWidget(label_server, 0, 0, 1, 1);
  gridLayout->addWidget(label_appname, 1, 0, 1, 1);
  gridLayout->addWidget(label_email, 2, 0, 1, 1);
  gridLayout->addWidget(label_apikey, 3, 0, 1, 1);
  gridLayout->addWidget(label_descriptor, 4, 0, 1, 1);
  
  gridLayout->addWidget(this->lineEdit_server , 0, 1, 1, 1);
  gridLayout->addWidget(this->lineEdit_appname, 1, 1, 1, 1);
  gridLayout->addWidget(this->lineEdit_email  , 2, 1, 1, 1);
  gridLayout->addWidget(this->lineEdit_apikey , 3, 1, 1, 1);
  gridLayout->addWidget(this->lineEdit_descriptor , 4, 1, 1, 1);

  verticalLayout->addLayout(gridLayout);

  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  verticalLayout->addWidget(buttonBox);

  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(process_accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(process_reject()));
}
