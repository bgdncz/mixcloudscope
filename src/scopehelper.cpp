#include "scopehelper.h"

using namespace std;
namespace http = core::net::http;
namespace net = core::net;

QJsonDocument ScopeHelper::getJson(const core::net::Uri::Path &path, const core::net::Uri::QueryParameters &parameters) {
    auto client = http::make_client();
    http::Request::Configuration configuration;
    net::Uri uri = net::make_uri("https://api.mixcloud.com", path, parameters);
    configuration.uri = client->uri_to_string(uri);
    configuration.header.add("User-Agent", "Ubuntu-Mixcloud-Scope");
    auto request = client->head(configuration);
    try {
        auto response = request->execute(bind(&ScopeHelper::progress_report, this, placeholders::_1));
        if (response.status != http::Status::ok) {
            throw domain_error(response.body);
        }
        return QJsonDocument::fromJson(response.body.c_str());
    } catch (net::Error &) {
        return QJsonDocument();
    }
}

ScopeHelper::CloudCasts ScopeHelper::parseJsonIntoCasts(const QJsonDocument &root) {
    QVariantMap variant = root.toVariant().toMap();
    CloudCasts result;
    for (const QVariant &i : variant["data"].toList()){
        QVariantMap item = i.toMap();
        QVariantMap pictures = item["pictures"].toMap();
        QVariantMap user = item["user"].toMap();
        QVariantMap userPics = user["pictures"].toMap();
        ScopeHelper::User myUser = User{
            user["name"].toString().toStdString(),
            userPics["medium"].toString().toStdString(),
            user["url"].toString().toStdString(),
            user["username"].toString().toStdString()
        };
        int audio_length = item["audio_length"].toInt();
        int favorite_count = item["favorite_count"].toInt();
        int listener_count = item["play_count"].toInt();
        int repost_count = item["repost_count"].toInt();
        string audiol_string;
        int hours, minutes, seconds;
        hours = floor((audio_length / 60 / 60) % 24);
        minutes = floor((audio_length / 60) % 60);
        seconds = floor(audio_length % 60);
        if (hours != 0) {
            audiol_string += std::to_string(hours) + "h";
        }
        if (minutes != 0) {
            audiol_string += std::to_string(minutes) + "m";
        }
        if (seconds != 0) {
            audiol_string += std::to_string(seconds) + "s";
        }
        result.emplace_back(
            CloudCast {
                item["name"].toString().toStdString(),
                pictures["large"].toString().toStdString(),
                item["url"].toString().toStdString(),
                audiol_string,
                favorite_count,
                listener_count,
                repost_count,
                myUser,
                item["slug"].toString().toStdString(),
                item["created_time"].toString().toStdString()
        });
    }
    return result;
}

ScopeHelper::Users ScopeHelper::getUsersByQuery(const std::string &query) {
    int cardinality = ScopeHelper::cardinality;
    QJsonDocument root;
    if (cardinality == 0) {
        root = getJson({"search/"}, {{"q", query}, {"type", "user"}});
    } else {
        root = getJson({"search/"}, {{"q", query}, {"type", "user"}, {"limit", std::to_string(cardinality)}});
    }
    QVariantMap rootVariant = root.toVariant().toMap();
    Users result;
    for (const QVariant &i : rootVariant["data"].toList()){
        QVariantMap item = i.toMap();
        string image = item["pictures"].toMap()["large"].toString().toStdString();
        result.emplace_back(
            User {
                item["name"].toString().toStdString(),
                image,
                item["url"].toString().toStdString(),
                item["username"].toString().toStdString()
            }
        );
    }
    return result;
}

std::vector<std::string> ScopeHelper::getExtra(int type, const std::string &url){
    std::vector<std::string> full;
    QJsonDocument root;
    root = getJson({url + "/"}, {});
    QVariantMap map = root.toVariant().toMap();
    switch(type){
        case 1:
            full.push_back(map["description"].toString().toStdString());
            break;
        case 2:
            full.push_back(map["city"].toString().toStdString());
            full.push_back(map["following_count"].toString().toStdString());
            full.push_back(map["country"].toString().toStdString());
            full.push_back(map["created_time"].toString().toStdString());
            full.push_back(map["cloudcast_count"].toString().toStdString());
            full.push_back(map["favorite_count"].toString().toStdString());
            full.push_back(map["follower_count"].toString().toStdString());
            full.push_back(map["biog"].toString().toStdString());
            break;
    }
    return full;
}


ScopeHelper::CloudCasts ScopeHelper::getCloudCastsByQuery(const std::string &query) {
    QJsonDocument root;
    int cardinality = ScopeHelper::cardinality;
    if (cardinality == 0) {
        root = getJson({"search/"}, {{"q", query}, {"type", "cloudcast"}});
    }
    else {
        root = getJson({"search/"}, {{"q", query}, {"type", "cloudcast"}, {"limit", std::to_string(cardinality)}});
    }
    return parseJsonIntoCasts(root);
}

ScopeHelper::CloudCasts ScopeHelper::getNew() {
    QJsonDocument root;
    int cardinality = ScopeHelper::cardinality;
    if (cardinality == 0) {
        root = getJson({"new/"}, {});
    }
    else {
        root = getJson({"new/"}, {{"limit", std::to_string(cardinality)}});
    }
    return parseJsonIntoCasts(root);
}

ScopeHelper::CloudCasts ScopeHelper::getHot() {
    QJsonDocument root;
    int cardinality = ScopeHelper::cardinality;
    if (cardinality == 0) {
        root = getJson({"popular/"}, {});
    }
    else {
        root = getJson({"popular/"}, {{"limit", std::to_string(cardinality)}});
    }
    return parseJsonIntoCasts(root);
}

http::Request::Progress::Next ScopeHelper::progress_report(const core::net::http::Request::Progress &progress) {
    (void)progress;
    return canceled_ ? http::Request::Progress::Next::abort_operation : http::Request::Progress::Next::continue_operation;
}

void ScopeHelper::cancel() {
    canceled_ = true;
}
