#ifndef API_CONFIG_H_
#define API_CONFIG_H_

#include <memory>
#include <string>

namespace api {

struct Config {
    typedef std::shared_ptr<Config> Ptr;

    /*
     * The root of all API request URLs
     */
    std::string installdir { "" };
    std::string apiroot { "http://api.mixcloud.com" };

    /*
     * The custom HTTP user agent string for this library
     */
    std::string user_agent { "Ubuntu-Mixcloud-Scope" };
};

}

#endif /* API_CONFIG_H_ */

