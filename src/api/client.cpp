#include <api/client.h>

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <unity/scopes/QueryMetadata.h>
#include <QVariantMap>
#include <math.h>
#include <QDebug>
#include <boost/algorithm/string/replace.hpp>


namespace http = core::net::http;
namespace net = core::net;

using namespace api;
using namespace std;

Client::Client(Config::Ptr config) :
    config_(config), cancelled_(false) {
}

std::vector<std::string> Client::getExtra(int type, const std::string &url){
    std::vector<std::string> full;
    QJsonDocument root;
    get({url + "/"}, {}, root);
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

void Client::get(const net::Uri::Path &path,
                 const net::Uri::QueryParameters &parameters, QJsonDocument &root) {
    // Create a new HTTP client
    auto client = http::make_client();

    // Start building the request configuration
    http::Request::Configuration configuration;

    // Build the URI from its components
    net::Uri uri = net::make_uri(config_->apiroot, path, parameters);
    std::string temporaryUri = client->uri_to_string(uri);
    configuration.uri = boost::replace_all_copy(temporaryUri, "%20", "+");
    qDebug(configuration.uri.c_str());
    // Give out a user agent string
    configuration.header.add("User-Agent", config_->user_agent);

    // Build a HTTP request object from our configuration
    auto request = client->head(configuration);

    try {
        // Synchronously make the HTTP request
        // We bind the cancellable callback to #progress_report
        auto response = request->execute(
                    bind(&Client::progress_report, this, placeholders::_1));

        // Check that we got a sensible HTTP status code
        if (response.status != http::Status::ok) {
            throw domain_error(response.body);
        }
        // Parse the JSON from the response
        root = QJsonDocument::fromJson(response.body.c_str());
    } catch (net::Error &) {
    }
}


Client::CloudCasts Client::parseJson(QJsonDocument root){
    QVariantMap variant = root.toVariant().toMap();
    CloudCasts result;
    for (const QVariant &i : variant["data"].toList()){
        QVariantMap item = i.toMap();
        QVariantMap pictures = item["pictures"].toMap();
        QVariantMap user = item["user"].toMap();
        QVariantMap userPics = user["pictures"].toMap();
        Client::User myUser = User{
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
        result.cloudcast.emplace_back(
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

Client::CloudCasts Client::getHot() {
    QJsonDocument root;
    int cardinality = Client::cardinality;
    if (cardinality == 0) {
        get({"popular/"}, {}, root);
    }
    else {
        get({"popular/"}, {{"limit", std::to_string(cardinality)}}, root);
    }
    return parseJson(root);
}

Client::CloudCasts Client::getNew(){
    QJsonDocument root;
    int cardinality = Client::cardinality;
    if (cardinality == 0) {
        get({"new/"}, {}, root);
    }
    else {
        get({"new/"}, {{"limit", std::to_string(cardinality)}}, root);
    }
    return parseJson(root);
}

Client::CloudCasts Client::getByQuery(const string &query){
    QJsonDocument root;
    int cardinality = Client::cardinality;
    if (cardinality == 0) {
        get({"search/"}, {{"q", query}, {"type", "cloudcast"}}, root);
    }
    else {
        get({"search/"}, {{"q", query}, {"type", "cloudcast"}, {"limit", std::to_string(cardinality)}}, root);
    }
    return parseJson(root);
}

Client::Users Client::getUsers(const string &query){
    QJsonDocument root;
    int cardinality = Client::cardinality;
    if (cardinality == 0){
        get({"search/"}, {{"q", query}, {"type", "user"}}, root);
    } else {
        get({"search/"}, {{"q", query}, {"type", "user"}, {"limit", std::to_string(cardinality)}}, root);
    }
    QVariantMap rootVariant = root.toVariant().toMap();
    Users result;
    for (const QVariant &i : rootVariant["data"].toList()){
        QVariantMap item = i.toMap();
        string image = item["pictures"].toMap()["large"].toString().toStdString();
        result.user.emplace_back(
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

http::Request::Progress::Next Client::progress_report(
        const http::Request::Progress&) {

    return cancelled_ ?
                http::Request::Progress::Next::abort_operation :
                http::Request::Progress::Next::continue_operation;
}

void Client::cancel() {
    cancelled_ = true;
}

Config::Ptr Client::config() {
    return config_;
}

