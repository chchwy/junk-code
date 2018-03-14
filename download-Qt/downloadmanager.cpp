#include "downloadmanager.h"

DownloadManager::DownloadManager()
{
    connect(&mManager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::doDownload(const QUrl& url)
{
    QNetworkRequest request(url);
    QNetworkReply* reply = mManager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    mCurrentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl& url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename))
    {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

bool DownloadManager::saveToDisk(const QString& filename, QIODevice* data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

void DownloadManager::Add(QString oneDownloadUrl)
{
    qDebug() << "  Add=" << oneDownloadUrl;
    mDownloadList.push_back(oneDownloadUrl);
}

void DownloadManager::execute()
{
    for (QString arg : mDownloadList)
    {
        QUrl url(arg);
        doDownload(url);
    }
}

void DownloadManager::sslErrors(const QList<QSslError>& sslErrors)
{
#ifndef QT_NO_SSL
    for(const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply* reply)
{
    QUrl url = reply->url();
    if (reply->error())
    {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    }
    else
    {
        QString filename = saveFileName(url);
        if (saveToDisk(filename, reply))
            qDebug("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
    }

    mCurrentDownloads.removeAll(reply);
    reply->deleteLater();

    if (mCurrentDownloads.isEmpty())
    {
        qDebug() << "Done";
    }
}
