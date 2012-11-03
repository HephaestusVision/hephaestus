#include <iostream>
#include <cassert>

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QScriptEngine>
#include <QVariant>
#include <QString>
#include <QApplication>
#include <QVariant>
#include <QMap>
#include <QString>
#include <QList>
#include <QUrl>

#include "MidasFolder.h"
#include "LoginDialog.h"
#include "PickFolder.h"
#include "MidasUpload.h"

/**
	 converts a JSON strinng into a QVariant structure.  VERY
	 UNSAFE---the evaluate method is vulnerable to arbitrary code
	 injections.!
	 
*/
QVariant parseJson(QByteArray & json) {
  QScriptEngine scriptEngine;
	QScriptValue result = scriptEngine.evaluate("(" + QString(json) + ")");
	if (scriptEngine.hasUncaughtException()){
		std::cerr << json.constData() << '\n';
		return QVariant(); // (! variant.isValid())
	}
  return result.toVariant();
}
/*
	
*/
QVariant getJson(const QUrl & url)
{
  QNetworkAccessManager networkAccessManager(NULL);
  QNetworkRequest networkRequest(url);
  QNetworkReply * networkReply = networkAccessManager.get(networkRequest);
  QEventLoop eventLoop;
  QObject::connect(networkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
  eventLoop.exec();
	QByteArray reply = networkReply->readAll();
	return parseJson(reply);
}

QVariant putJson(
  const QUrl & url, const QString & filename)
{
  QNetworkAccessManager networkAccessManager(NULL);
  QNetworkRequest networkRequest(url);
	networkRequest.setHeader(
		QNetworkRequest::ContentTypeHeader, "application/octet-stream");
  QFile infile(filename);
  infile.open( QIODevice::ReadOnly );
  QNetworkReply * networkReply
		= networkAccessManager.post(networkRequest,&infile);
  QEventLoop eventLoop;
  QObject::connect(networkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
  eventLoop.exec();
	QByteArray reply = networkReply->readAll();
	return parseJson(reply);
}

/**
	 for debugging.
*/
std::ostream & print(std::ostream & o, QVariant const & qvariant, int n) {
  for (int j = 0; j < (4 * n) ; j++) o << ' ';
	if (qvariant.isNull())
		return o << "NULL\n";
	if (!qvariant.isValid())
		return o << "<invalid>\n";
  switch(qvariant.type())
    {
    case QVariant::Bool:
      return o << (qvariant.toBool() ? "True": "False") << '\n';
    case QVariant::Double:
      return o << qvariant.toDouble() << '\n';
    case QVariant::Int:
      return o << qvariant.toInt() << '\n';
    case QVariant::String:
      return o << '"' << qvariant.toString().toUtf8().constData() << "\"\n";
    case QVariant::Map:
      o << "map:" << '\n';
      for(QMapIterator<QString, QVariant> iterator(qvariant.toMap());
          iterator.hasNext(); )
        {
          iterator.next();
          for (int k = 0; k < (4 * n + 2) ; k++) o << ' ';
          o << '"' << iterator.key().toUtf8().constData() << '"' << ":\n";
          print(o, iterator.value(), n + 1);
        }
			break;
    case QVariant::List:
      {
        o << "list:" << '\n';
        QList<QVariant> vlist = qvariant.toList();
        for (int i = 0; i < vlist.size(); ++i)
          print(o, vlist.at(i), n + 1);
      }
			break;
    default:
			o << qvariant.typeName () << '\t';
			return o << "????" << '\n';
    }
	return o;
}

/**
  checks the return status of a getJson call.  Failed calls whill
  return a invalid QVariant.  Then MIDAS api always returns a "stat"
  field which is set to "ok" if somethng goes wrong. 
 */
bool isokay(QVariant & v, QString what) {
  if (v.isValid() && 
      (v.type() == QVariant::Map) &&
      (v.toMap().count("stat") > 0) &&
      (v.toMap()["stat"].type() == QVariant::String) &&
      (v.toMap()["stat"].toString() == "ok"))
    return true;
  else
    {
    print (std::cerr, v);
    std::cerr << qPrintable(what) << " failed.\n";
    return false;
    }
}

/**
	 Given a set of login information, get a list of visible folders on
	 the server.
*/
QList< MidasFolder > get_list_of_folders(
  QString server, QString appname,
  QString email, QString appkey)
{
  QList< MidasFolder > list_of_folders;

  QUrl url;
  url.setUrl(server);
  url.addQueryItem("method","midas.version");
	#ifdef VERBOSE
	std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  QVariant r = getJson(url);
  if (! isokay(r, QString("connecting to server at ") + server))
    return list_of_folders;

  url.setUrl(server);
  url.addQueryItem("method","midas.login");
  url.addQueryItem("appname", appname);
  url.addQueryItem("email", email);
  url.addQueryItem("apikey", appkey);
	#ifdef VERBOSE
  std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  r = getJson(url);
  if (! isokay(r, "midas.login"))
    return list_of_folders;

  QString token = r.toMap()["data"].toMap()["token"].toString();

  url.setUrl(server);
  url.addQueryItem("method","midas.community.list");
  url.addQueryItem("token", token);
	#ifdef VERBOSE
  std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  r = getJson(url);
  if (! isokay(r, "midas.community.list"))
    return list_of_folders;
  QList< QVariant > data = r.toMap()["data"].toList();

  for (int i = 0; i < data.size(); ++i)
    {
    QString community_name = data[i].toMap()["name"].toString();
    QString community_id = data[i].toMap()["community_id"].toString();
    url.setUrl(server); // get list of folders
    url.addQueryItem("method","midas.community.children"); 
    url.addQueryItem("token", token);
    url.addQueryItem("id", community_id);
    #ifdef VERBOSE
		std::cerr << qPrintable(url.toString()) << "\n\n";
	  #endif
    r = getJson(url);
    if (! isokay(r, "midas.community.children"))
      return list_of_folders;
    QList< QVariant > folder_list = 
      r.toMap()["data"].toMap()["folders"].toList();
    for (int j = 0; j < folder_list.size(); ++j)
      {
      QString folder_name = folder_list[j].toMap()["name"].toString();
      QString folder_id = folder_list[j].toMap()["folder_id"].toString();
      QString fullname = community_name + "/" + folder_name;
      list_of_folders.append(MidasFolder(
        server, token, folder_id, fullname, true));
      }
    }
  
  url.setUrl(server);
  url.addQueryItem("method","midas.user.folders");
  url.addQueryItem("token", token);
	#ifdef VERBOSE
  std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  r = getJson(url);
  if (! isokay(r, "midas.user.folders"))
    return list_of_folders;
  data = r.toMap()["data"].toList();
  for (int i = 0; i < data.size(); ++i)
    {
    QString name = data[i].toMap()["name"].toString();
    QString folder_id = data[i].toMap()["folder_id"].toString();
    list_of_folders.append(MidasFolder(
      server, token, folder_id, name, false));
    }
  return list_of_folders;
}


void runDialog(QDialog * dialog) {
	assert(dialog != NULL);
  QEventLoop eventLoop;
  QObject::connect(dialog, SIGNAL(finished(int)), &eventLoop, SLOT(quit()));
	dialog->show();
  dialog->raise();
  dialog->activateWindow();
  eventLoop.exec();
}

bool upload_to_midas_folder(const MidasFolder * folder,
														QString & filename,
														QString & fileDescriptor) {
	QString file_size = QString::number(QFileInfo(filename).size());

  QUrl url;
  url.setUrl(folder->server);
  url.addQueryItem("method","midas.upload.generatetoken");
  url.addQueryItem("token",folder->token);
  url.addQueryItem("folderid",folder->id);
  url.addQueryItem("itemname",fileDescriptor);
  url.addQueryItem("filename",fileDescriptor);
	#ifdef VERBOSE
  std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  QVariant r = getJson(url);
  if (! isokay(r, "midas.upload.generatetoken"))
    return false;
  QString uploadtoken = r.toMap()["data"].toMap()["token"].toString();

  url.setUrl(folder->server);
  url.addQueryItem("method","midas.upload.perform");
  url.addQueryItem("uploadtoken",uploadtoken);
  url.addQueryItem("filename",fileDescriptor);
  url.addQueryItem("length",file_size);
	#ifdef VERBOSE
  std::cerr << qPrintable(url.toString()) << "\n\n";
	#endif
  r = putJson(url,filename);
  return isokay(r, "midas.upload.perform");
}

void upload_to_midas(LoginDialog * loginDialog, QString & filename) {
	runDialog(loginDialog);
	if (!loginDialog->isGood()) {
		std::cerr << "! loginDialog->isGood()\n\n";
		return;
	}
	QString server  = loginDialog->server();
  QString appname = loginDialog->appname();
  QString email   = loginDialog->email();
  QString apikey  = loginDialog->apikey();
  QString fileDescriptor  = loginDialog->descriptor();

	QList< MidasFolder > list_of_folders = 
		get_list_of_folders(server, appname, email, apikey);

	//  PickFolder pick_folder(loginDialog->parent(), &list_of_folders);
  PickFolder pick_folder(NULL, &list_of_folders);
	runDialog(&pick_folder);

  const MidasFolder * folder = pick_folder.getFolder();
  if (folder == NULL) {
		std::cerr << "PickFolder failed\n\n";
    return;
	}
	upload_to_midas_folder(folder, filename, fileDescriptor);
}
