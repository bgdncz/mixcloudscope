#ifndef API_CLIENT_H_
#define API_CLIENT_H_

#include <api/config.h>

#include <atomic>
#include <deque>
#include <map>
#include <string>
#include <core/net/http/request.h>
#include <core/net/uri.h>

#include <QJsonDocument>

namespace api {

/**
 * Provide a nice way to access the HTTP API.
 *
 * We don't want our scope's code to be mixed together with HTTP and JSON handling.
 */
class Client {
public:

    /**
     * Information about a City
     */
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

    /**
     * Temperature information for a day.
     */
    typedef std::deque<CloudCast> CloudCastList;
    typedef std::deque<User> UserList;

    struct CloudCasts {
        CloudCastList cloudcast;
    };
    struct Users {
        UserList user;
    };

    int cardinality;

    /**
     * Weather information for a day.
     */

    /**
     * A list of weather information
     */

    /**
     * Weather information about the current day
     */

    Client(Config::Ptr config);

    virtual ~Client() = default;

    /**
     * Get the current weather for the specified location
     */

    virtual CloudCasts getHot();
    virtual CloudCasts getByQuery(const std::string &query);
    virtual Users getUsers(const std::string &query);
    virtual CloudCasts parseJson(QJsonDocument root);
    virtual CloudCasts getNew();
    virtual std::vector<std::string> getExtra(int type, const std::string &url);
    /**
     * Cancel any pending queries (this method can be called from a different thread)
     */
    virtual void cancel();

    virtual Config::Ptr config();

protected:
    void get(const core::net::Uri::Path &path,
             const core::net::Uri::QueryParameters &parameters,
             QJsonDocument &root);
    /**
     * Progress callback that allows the query to cancel pending HTTP requests.
     */
    core::net::http::Request::Progress::Next progress_report(
            const core::net::http::Request::Progress& progress);

    /**
     * Hang onto the configuration information
     */
    Config::Ptr config_;

    /**
     * Thread-safe cancelled flag
     */
    std::atomic<bool> cancelled_;
};

}

#endif // API_CLIENT_H_

