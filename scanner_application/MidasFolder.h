// #include "MidasFolder.h"
#ifndef MIDASFOLDER_H
#define MIDASFOLDER_H

#include <QString>

class MidasFolder {
public:
  QString server;
  QString token;
  QString id;
  QString name;
  bool is_community_folder;
  MidasFolder(
    QString & server, QString & token, QString & id,
    QString & name, bool is_community_folder) :
    server(server), token(token), id(id), name(name),
    is_community_folder(is_community_folder) { }
};

#endif /* MIDASFOLDER_H */
