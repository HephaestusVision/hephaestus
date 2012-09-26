#include "Cloudy.h"

#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>

static void alert(const char * s)
{
  if (QCoreApplication::instance() == NULL)
    {
    int a = 0;
    char * ptr = NULL;
    QApplication * q = new QApplication(a, &ptr);
    }
  QMessageBox::about(NULL, "Hephaestus", s);
}

Cloudy::Cloudy(/* FIXME: constructor arguments? */) { }
Cloudy::~Cloudy() { }
PointCloud const * Cloudy::GetCurrentPointCloud() const { return NULL; }
void Cloudy::UpdatePointCloud() { alert("update"); }
void Cloudy::CreatePointCloud() { alert("create"); }
void Cloudy::ClearPointCloud() { alert("clear"); }


