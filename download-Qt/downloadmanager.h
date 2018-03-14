#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>
#include <QUrl>

class QSslError;

class DownloadManager: public QObject
{
    Q_OBJECT
public:
    DownloadManager();
    void doDownload(const QUrl& url);
    QString saveFileName(const QUrl& url);
    bool saveToDisk(const QString& filename, QIODevice* data);

    void Add(QString oneDownloadUrl);

public slots:
    void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);

private:
    QNetworkAccessManager mManager;
    QList<QNetworkReply*> mCurrentDownloads;

    std::vector<QString> mDownloadList;
};

#endif // DOWNLOADMANAGER_H
