#include "query.h"
#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/SearchMetadata.h>
#include <unity/scopes/VariantBuilder.h>
#include <iostream>

namespace sc = unity::scopes;

using namespace std;
using namespace scope;

Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata) :
    sc::SearchQueryBase(query, metadata) {
}

void Query::cancelled() {
    scopeHelper.cancel();
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
        scopeHelper.cardinality = myData.cardinality();
        string query_string = query.query_string();
        if (query_string.empty()){
            ScopeHelper::CloudCasts hotCasts;
            ScopeHelper::CloudCasts newCasts;
            hotCasts = scopeHelper.getHot();
            newCasts = scopeHelper.getNew();
            auto hot_category = reply->register_category("hotCasts", ScopeHelper::_("Popular"), "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            auto new_category = reply->register_category("newCasts", ScopeHelper::_("New"), "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            for (const auto &hotCast : hotCasts){
                sc::CategorisedResult hotRes(hot_category);
                parseCasts(hotRes, hotCast);
                if (!reply->push(hotRes)){
                    return;
                }
            }
            if (showNew){
                for (const auto &newCast : newCasts){
                    sc::CategorisedResult newRes(new_category);
                    parseCasts(newRes, newCast);
                    if (!reply->push(newRes)){
                        return;
                    }
                }
            }
        } else {
            ScopeHelper::CloudCasts castResults;
            ScopeHelper::Users userResults;
            castResults = scopeHelper.getCloudCastsByQuery(query_string);
            userResults = scopeHelper.getUsersByQuery(query_string);
            auto cast_category = reply->register_category("casts", ScopeHelper::_("Cloudcasts"), "", sc::CategoryRenderer(TRACKS_TEMPLATE));
            auto user_category = reply->register_category("users", ScopeHelper::_("Users"), "", sc::CategoryRenderer(USERS_TEMPLATE));
            for (const auto &cast : castResults){
                sc::CategorisedResult castRes(cast_category);
                parseCasts(castRes, cast);
                if (!reply->push(castRes)){
                    return;
                }
            }
            for (const auto &user : userResults){
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

void Query::parseCasts(sc::CategorisedResult &hotRes, const ScopeHelper::CloudCast &hotCast){
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
    hotRes["created"] = hotCast.created;
    string myValue;
    if (hotCast.favorite_count != 0) {
        myValue = std::to_string(hotCast.favorite_count) + " ♥";
    }
    else {
        myValue = std::to_string(hotCast.listener_count) + " ♪♫";
    }
    sc::VariantBuilder builder;
    builder.add_tuple({{"value", sc::Variant("⌚ " + hotCast.audio_length)}});
    builder.add_tuple({{"value", sc::Variant(myValue)}});
    hotRes["attributes"] = builder.end();
}


