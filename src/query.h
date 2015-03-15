#ifndef SCOPE_QUERY_H_
#define SCOPE_QUERY_H_
#define TRACKS_TEMPLATE "{\"schema-version\":1,\"template\":{\"category-layout\":\"grid\",\"card-layout\":\"horizontal\",\"card-size\":\"medium\"},\"components\":{\"title\":\"title\",\"art\":{\"field\":\"art\"},\"subtitle\":\"subtitle\",\"attributes\":\"attributes\"}}"
#define USERS_TEMPLATE "{\"schema-version\":1,\"components\":{\"title\":\"title\",\"art\":{\"field\":\"art\"}, \"subtitle\": \"subtitle\", \"attributes\": \"attributes\", \"emblem\": \"emblem\"}}"

#include "scopehelper.h"
#include <unity/scopes/SearchQueryBase.h>
#include <unity/scopes/ReplyProxyFwd.h>

namespace scope {

/**
 * Represents an individual query.
 *
 * A new Query object will be constructed for each query. It is
 * given query information, metadata about the search, and
 * some scope-specific configuration.
 */
class Query: public unity::scopes::SearchQueryBase {
public:
    Query(const unity::scopes::CannedQuery &query,
          const unity::scopes::SearchMetadata &metadata);

    ~Query() = default;

    void cancelled() override;

    void run(const unity::scopes::SearchReplyProxy &reply) override;
    void parseCasts(unity::scopes::CategorisedResult &hotRes, const ScopeHelper::CloudCast &hotCast);

private:
    ScopeHelper scopeHelper;
    void initScope();
    bool showNew;
    bool getExtra;
};

}

#endif // SCOPE_QUERY_H_


