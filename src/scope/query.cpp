#include <boost/algorithm/string/trim.hpp>
#include <algorithm>
#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/SearchMetadata.h>
#include <unity/scopes/VariantBuilder.h>

namespace sc = unity::scopes;
namespace alg = boost::algorithm;

using namespace std;
using namespace api;
using namespace scope;


/**
 * Define the layout for the forecast results
 *
 * The icon size is small, and ask for the card layout
 * itself to be horizontal. I.e. the text will be placed
 * next to the image.
 */
/**
 * Define the larger "current weather" layout.
 *
 * The icons are larger.
 */
const static string TRACKS_TEMPLATE = "{\"schema-version\":1,\"template\":{\"category-layout\":\"grid\",\"card-layout\":\"horizontal\",\"card-size\":\"medium\"},\"components\":{\"title\":\"title\",\"art\":{\"field\":\"art\"},\"subtitle\":\"subtitle\",\"attributes\":\"attributes\",\"emblem\":\"emblem\"}}";
const static string USERS_TEMPLATE = "{\"schema-version\":1,\"components\":{\"title\":\"title\",\"art\":{\"field\":\"art\"}, \"subtitle\": \"subtitle\", \"attributes\": \"attributes\", \"emblem\": \"emblem\"}}";
Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata,
             Config::Ptr config) :
    sc::SearchQueryBase(query, metadata), client_(config) {
    Query::installdir = config->installdir;
}

void Query::cancelled() {
    client_.cancel();
}

void Query::initScope() {
    unity::scopes::VariantMap config = settings();
    showNew = config["showNew"].get_bool();
    getExtra = config["getExtra"].get_bool();
}


void Query::run(sc::SearchReplyProxy const& reply) {
    try {
        initScope();
        const sc::CannedQuery &query(sc::SearchQueryBase::query());
        sc::SearchMetadata myData = sc::SearchQueryBase::search_metadata();
        client_.cardinality = myData.cardinality();
        string query_string = alg::trim_copy(query.query_string());
        string emblemPath = Query::installdir + "/mixcloud.svg";
        if (query_string.empty()){
            Client::CloudCasts hotCasts;
            Client::CloudCasts newCasts;
            hotCasts = client_.getHot();
            newCasts = client_.getNew();
            auto hot_category = reply->register_category("hotCasts", "Popular", "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            auto new_category = reply->register_category("newCasts", "New", "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            for (const auto &hotCast : hotCasts.cloudcast){
                sc::CategorisedResult hotRes(hot_category);
                hotRes.set_uri(hotCast.url);
                hotRes.set_title(hotCast.name);
                hotRes.set_art(hotCast.thumbnail);
                hotRes["subtitle"] = hotCast.user.name;
                hotRes["username"] = hotCast.user.username;
                hotRes["type"] = "cloudcast";
                hotRes["getExtra"] = getExtra;
                hotRes["repost"] = hotCast.repost_count;
                hotRes["slug"] = hotCast.slug;
                hotRes["listen"] =  hotCast.listener_count;
                hotRes["favorite"] = hotCast.favorite_count;
                hotRes["audio_length"] = hotCast.audio_length;
                string myValue = "♥ " + std::to_string(hotCast.favorite_count);
                sc::VariantBuilder builder;
                builder.add_tuple({{"value", sc::Variant(hotCast.audio_length)}});
                builder.add_tuple({{"value", sc::Variant(myValue)}});
                hotRes["attributes"] = builder.end();
                hotRes["emblem"] = emblemPath;
                if (!reply->push(hotRes)){
                    return;
                }
            }
            if (showNew){
                for (const auto &newCast : newCasts.cloudcast){
                    sc::CategorisedResult newRes(new_category);
                    newRes.set_uri(newCast.url);
                    newRes.set_title(newCast.name);
                    newRes.set_art(newCast.thumbnail);
                    newRes["subtitle"] = newCast.user.name;
                    newRes["type"] = "cloudcast";
                    newRes["slug"] = newCast.slug;
                    newRes["repost"] = newCast.repost_count;
                    newRes["username"] = newCast.user.username;
                    newRes["listen"] =  newCast.listener_count;
                    newRes["favorite"] = newCast.favorite_count;
                    newRes["audio_length"] = newCast.audio_length;
                    newRes["getExtra"] = getExtra;
                    string myValue = "♪♫" + std::to_string(newCast.listener_count);
                    sc::VariantBuilder builder;
                    builder.add_tuple({{"value", sc::Variant(newCast.audio_length)}});
                    builder.add_tuple({{"value", sc::Variant(myValue)}});
                    newRes["attributes"] = builder.end();
                    newRes["emblem"] = emblemPath;
                    if (!reply->push(newRes)){
                        return;
                    }
                }
            }
        } else {
            Client::CloudCasts castResults;
            Client::Users userResults;
            castResults = client_.getByQuery(query_string);
            userResults = client_.getUsers(query_string);
            auto cast_category = reply->register_category("casts", "Cloudcasts", "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            auto user_category = reply->register_category("users", "Users", "", sc::CategoryRenderer(USERS_TEMPLATE));
            for (const auto &cast : castResults.cloudcast){
                sc::CategorisedResult castRes(cast_category);
                castRes.set_title(cast.name);
                castRes.set_uri(cast.url);
                castRes.set_art(cast.thumbnail);
                castRes["subtitle"] = cast.user.name;
                castRes["repost"] = cast.repost_count;
                castRes["listen"] =  cast.listener_count;
                castRes["favorite"] = cast.favorite_count;
                castRes["type"] = "cloudcast";
                castRes["username"] = cast.user.username;
                castRes["slug"] = cast.slug;
                castRes["getExtra"] = getExtra;
                castRes["audio_length"] = cast.audio_length;
                string myValue = "♥ " + std::to_string(cast.favorite_count);
                sc::VariantBuilder builder;
                builder.add_tuple({{"value", sc::Variant(cast.audio_length)}});
                builder.add_tuple({{"value", sc::Variant(myValue)}});
                castRes["attributes"] = builder.end();
                castRes["emblem"] = emblemPath;
                if (!reply->push(castRes)){
                    return;
                }
            }
            for (const auto &user : userResults.user){
                sc::CategorisedResult userRes(user_category);
                userRes.set_title(user.name);
                userRes.set_uri(user.url);
                userRes.set_art(user.thumbnail);
                userRes["subtitle"] = user.username;
                userRes["type"] = "user";
                userRes["getExtra"] = getExtra;
                if (!reply->push(userRes)){
                    return;
                }
            }
        }

    } catch (domain_error &e) {
        // Handle exceptions being thrown by the client API
        cerr << e.what() << endl;
        reply->error(current_exception());
    }
}

