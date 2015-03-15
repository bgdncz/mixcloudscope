#ifndef SCOPEHELPER_H
#define SCOPEHELPER_H

#include <QJsonDocument>
#include <libintl.h>
#include <core/net/http/request.h>
#include <core/net/uri.h>
#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <atomic>
#include <deque>
#include <QVariantMap>

class ScopeHelper
{
public:
    struct User {
        std::string name;
        std::string thumbnail;
        std::string url;
        std::string username;
    };
    struct CloudCast {
        std::string name;
        std::string thumbnail;
        std::string url;
        std::string audio_length;
        int favorite_count;
        int listener_count;
        int repost_count;
        User user;
        std::string slug;
        std::string created;
    };
    typedef std::deque<CloudCast> CloudCasts;
    typedef std::deque<User> Users;
    int cardinality;

    ScopeHelper(){}
    CloudCasts getHot();
    CloudCasts getNew();
    CloudCasts getCloudCastsByQuery(const std::string& query);
    CloudCasts parseJsonIntoCasts(const QJsonDocument& root);
    Users getUsersByQuery(const std::string& query);
    std::vector<std::string> getExtra(int type, const std::string &url);
    void cancel();
    static inline char* _(const char* __msgid) {
        return dgettext("com.ubuntu.developer.mixcloud", __msgid);
    }

private:
    QJsonDocument getJson(const core::net::Uri::Path &path, const core::net::Uri::QueryParameters &parameters);
    core::net::http::Request::Progress::Next progress_report(const core::net::http::Request::Progress &progress);

    std::atomic<bool> canceled_;
};

#endif // SCOPEHELPER_H
