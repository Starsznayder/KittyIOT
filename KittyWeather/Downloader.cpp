#include "Downloader.h"
#include <QThread>
#include <QNetworkReply>
#include <QTimer>
#include <kittyLogs/log.h>

#include "Config.h"

namespace kitty{

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    finish_(false)
{
    manager = new QNetworkAccessManager(this);
}

void Downloader::startStatusUpdateThread()
{
    finish_.store(false);
    connect(this, SIGNAL(gotJSON()), this, SLOT(readJSON()));
    emit gotJSON();
}

void Downloader::readJSON()
{
    QNetworkRequest request;
    QUrl url(Config::instance().dataWebsite.dataServerAddr.get().c_str());
    request.setUrl(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onJSON()));
}

void Downloader::onJSON()
{
    _KI("Weather", "downloading...");
    QNetworkReply* reply = qobject_cast<QNetworkReply*> (QObject::sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        _KE("Weather", "Error downloading. " << reply->errorString().toStdString());
    }
    else
    {
        QString json = reply->readAll();
        if(json.size() > 1)
        {
            emit jsonObject(QJsonDocument::fromJson(json.toUtf8()).object());
        }
        disconnect(reply, SIGNAL(finished()), this, SLOT(onJSON()));
        delete reply;
    }

    if (!finish_.load())
    {
        QTimer::singleShot(Config::instance().dataWebsite.interval.get() * 1000, this, SLOT(readJSON()));
    }
}

void Downloader::finish()
{
    finish_.store(true);
}

Downloader::~Downloader()
{
    finish_.store(true);
}

}
