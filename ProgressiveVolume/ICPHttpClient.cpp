#include "ICPHttpClient.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QFile>
#include <QHttpMultiPart>

QNetworkAccessManager* ICPHttpClient::_networkAccessManager = NULL;


ICPHttpClient::ICPHttpClient(QString url)
{
    _httpReturnCode = 0;
    _url = url;
    _hasError = false;
    _downloadToFile = false;

    _downloadFile = NULL;
    createNetworkAccessManager();
}

ICPHttpClient::~ICPHttpClient()
{
    //qDebug() << "deleted";
}

inline QString GetRandomString()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters

    QString randomString;
    for(int i=0; i<randomStringLength; ++i) {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

void ICPHttpClient::prepareDownloadFile(QNetworkReply* reply)
{
    static const char* FILE_NAME = "filename";

    if(_downloadFile != NULL && _downloadFile->isOpen())
        return;

    QString downloadFileName;
    if(_reply->hasRawHeader("Content-Disposition")) {
        QString deposition = _reply->rawHeader("Content-Disposition");
        int index = deposition.indexOf(FILE_NAME);
        if(index > 0) {
            downloadFileName = deposition.right(deposition.length() - index - (strlen(FILE_NAME) + 1)).replace("\"", "");
        }
    }

    if(downloadFileName.length() == 0)
        downloadFileName = GetRandomString();

    QString fullPath(_downloadPath + "/" + downloadFileName);

    if(QFile::exists(fullPath)) {
        QFile::remove(fullPath);
    }

    _downloadFile = new QFile(fullPath, this);
    _downloadFile->open(QIODevice::WriteOnly);
}

void ICPHttpClient::httpReadyRead()
{
    if(_downloadToFile) {
        prepareDownloadFile(_reply);

        if(_downloadFile != NULL && _downloadFile->isOpen()) {
            _downloadFile->write(_reply->readAll());
        }
    } else {
        //if(_reply->hasRawHeader("Content-Type")) {
        //    QString contentType = QString::fromLatin1(_reply->rawHeader("Content-Type"));
        //}
        _response = _response.append(QString::fromUtf8(_reply->readAll()));
    }
}

void ICPHttpClient::httpFinished()
{
    _httpReturnCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    _reply->deleteLater();

    if(_downloadToFile) {
        if(_downloadFile != NULL && _downloadFile->isOpen()) {
            _downloadFile->flush();
            _downloadFile->close();
            _response = _downloadFile->fileName();
        }

        if(_hasError) {
            if(_downloadFile)
                _downloadFile->remove();
        }
    }

    if(_finishFunc) {
        ICPHttpResult result(this);
        _finishFunc(result);

        this->deleteLater();
    }
}

void ICPHttpClient::httpError(QNetworkReply::NetworkError netError)
{
    _errorString = _reply->errorString();
    _hasError = true;
}

void ICPHttpClient::setUrl(QString url)
{
    _url = url;
}

void ICPHttpClient::createNetworkAccessManager()
{
    if(_networkAccessManager == NULL)
        _networkAccessManager = new QNetworkAccessManager();
}

void ICPHttpClient::post(QString content)
{
    QUrl url(_url);
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    prepareRequest(request);
    QNetworkReply *reply = _networkAccessManager->post(request, content.toUtf8());
    bindReplyConnect(reply);

    _reply = reply;
}

void ICPHttpClient::post(QHttpMultiPart* multiPartData)
{
    QUrl url(_url);
    QNetworkRequest request(url);
    prepareRequest(request);
    QNetworkReply *reply = _networkAccessManager->post(request, multiPartData);
    multiPartData->setParent(reply);
    bindReplyConnect(reply);

    _reply = reply;
}

void ICPHttpClient::put(QString content)
{
    QUrl url(_url);
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    prepareRequest(request);
    QNetworkReply *reply = _networkAccessManager->put(request, content.toUtf8());
    bindReplyConnect(reply);

    _reply = reply;
}

void ICPHttpClient::get()
{
    QUrl url(_url);
    QNetworkRequest request(url);
    prepareRequest(request);
    QNetworkReply *reply = _networkAccessManager->get(request);
    bindReplyConnect(reply);

    _reply = reply;
}

void ICPHttpClient::deleteResource()
{
    QUrl url(_url);
    QNetworkRequest request(url);
    prepareRequest(request);
    QNetworkReply *reply = _networkAccessManager->deleteResource(request);
    bindReplyConnect(reply);

    _reply = reply;
}

void ICPHttpClient::setHeader(QString headerName, QString headerContent)
{
    _headerMap[headerName] = headerContent;
}

void ICPHttpClient::prepareRequest(QNetworkRequest& request)
{
    QMap<QString, QString>::const_iterator i = _headerMap.constBegin();
    while(i != _headerMap.constEnd()) {
        request.setRawHeader(i.key().toLocal8Bit(), i.value().toUtf8());
        ++i;
    }
}

void ICPHttpClient::bindReplyConnect(QNetworkReply* reply)
{
    connect(reply, SIGNAL(finished()), this,
            SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this,
            SLOT(httpReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
            SLOT(httpError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this,
            SLOT(httpDownloadProgress(qint64, qint64)));
}

void ICPHttpClient::httpDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(_progressFunc)
        _progressFunc(_url, bytesReceived, bytesTotal);
}


