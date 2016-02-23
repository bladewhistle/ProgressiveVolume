#ifndef HTTPREPLYDATA_H
#define HTTPREPLYDATA_H

#include <QNetworkReply>
#include <functional>
#include "ICPHttpResult.h"

class QNetworkAccessManager;
class QFile;
class QHttpMultiPart;
class ICPHttpClient;


class ICPHttpClient: public QObject
{
    Q_OBJECT

public:
    ICPHttpClient(QString url);
    ~ICPHttpClient();

    void setUrl(QString url);
    void post(QString content);
    void post(QHttpMultiPart* multiPartData);
    void put(QString content);
    void get();
    void deleteResource();
    void setHeader(QString headerName, QString headerContent);

    void setDownloadPath(QString path)
    {
        this->_downloadPath = path;
        _downloadToFile = true;
    }

    QString getResult() const
    {
        return _response;
    }

    QString getError() const
    {
        return _errorString;
    }

    bool isOk() const
    {
        return _httpReturnCode == ICPHttpResult::OK;
    }

    bool getHasError() const
    {
        return _hasError;
    }

    int getHttpReturnCode() const
    {
        return _httpReturnCode;
    }

    QString getUrl() const
    {
        return _url;
    }

    QString getIdentifier() const
    {
        return _identifier;
    }
    void setIdentifier(QString identifier)
    {
        _identifier = identifier;
    }

    void setFinishFuc(std::function<void(ICPHttpResult&)> func)
    {
        _finishFunc = func;
    }

    void setProgressFunc(std::function<void(QString url, qint64 bytesReceived, qint64 bytesTotal)> func)
    {
        _progressFunc = func;
    }

protected slots:
    void httpFinished();
    void httpReadyRead();
    void httpError(QNetworkReply::NetworkError netError);
    void httpDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    static void createNetworkAccessManager();
    void bindReplyConnect(QNetworkReply* reply);
    void prepareRequest(QNetworkRequest& request);
    void prepareDownloadFile(QNetworkReply* reply);

protected:
    int _httpReturnCode;
    bool _hasError;
    bool _downloadToFile;

    QString _response;
    QString _errorString;
    QString _url;
    QString _downloadPath;
    QString _identifier;

    QNetworkReply* _reply;
    QFile* _downloadFile;
    QMap<QString, QString> _headerMap;

    std::function<void(ICPHttpResult&)> _finishFunc;
    std::function<void(QString, qint64, qint64)> _progressFunc;

    static QNetworkAccessManager* _networkAccessManager;
};

#endif // HTTPREPLYDATA_H
