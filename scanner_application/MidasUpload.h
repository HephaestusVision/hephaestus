#ifndef QTP_H
#define QTP_H

#include <iostream>
class QString;
class QUrl;
class QVariant;
class MidasFolder;

QVariant parseJson(QByteArray & json);
QVariant getJson(const QUrl & url);
QVariant putJson(const QUrl & url, const QString & filename);
std::ostream & print(std::ostream &, QVariant const &, int n = 0);


/**
  checks the return status of a getJson call.  Failed calls whill
  return a invalid QVariant.  Then MIDAS api always returns a "stat"
  field which is set to "ok" if somethng goes wrong. 
 */
bool isokay(QVariant & v, QString what);

/**
	 Given a set of login information, get a list of visible folders on
	 the server.
*/
QList< MidasFolder > get_list_of_folders(
  QString server, QString appname,
  QString email, QString appkey);

/* waits till a dialog emits the finished() signal */
void runDialog(QDialog * dialog);

/** perform upload */
bool upload_to_midas_folder(const MidasFolder * folder,
														QString & filename,
														QString & fileDescriptor);

/** The fnction you want to call */
void upload_to_midas(LoginDialog * loginDialog, QString & filename);


#endif /* QTP_H */



