#ifndef KITTYDOWNLOADER_H
#define KITTYDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QVector>
#include <QFuture>
#include <atomic>

namespace kitty
{

class Downloader : public QObject
{
    Q_OBJECT

public:
    Downloader(QObject *parent = 0);
    ~Downloader();

public slots:
    void startStatusUpdateThread();
    void finish();

signals:
    void gotJSON();
    void jsonObject(QJsonObject);
 
private:
    std::atomic<bool> finish_;
    QNetworkAccessManager* manager;
    void worker();
    QFuture<void> readerThreadObject;

private slots:
    void onJSON();
    void readJSON();


};


} // namespace kitty

#endif // MAINWINDOW_H
