#include "preview.h"
#include "query.h"
#include "scope.h"

namespace sc = unity::scopes;
using namespace std;
using namespace scope;

void Scope::start(string const&) {

    setlocale(LC_ALL, "");
    string translation_directory = ScopeBase::scope_directory() + "/../share/locale/";
    bindtextdomain("com.ubuntu.developer.boghison.mixcloud", translation_directory.c_str());
}

void Scope::stop() {
}

sc::SearchQueryBase::UPtr Scope::search(const sc::CannedQuery &query,
                                        const sc::SearchMetadata &metadata) {
    // Boilerplate construction of Query
    return sc::SearchQueryBase::UPtr(new Query(query, metadata));
}

sc::PreviewQueryBase::UPtr Scope::preview(sc::Result const& result,
                                          sc::ActionMetadata const& metadata) {
    // Boilerplate construction of Preview
    auto myPreview = new Preview(result, metadata);
    return sc::PreviewQueryBase::UPtr(myPreview);
}

#define EXPORT __attribute__ ((visibility ("default")))

// These functions define the entry points for the scope plugin
extern "C" {

EXPORT
unity::scopes::ScopeBase*
// cppcheck-suppress unusedFunction
UNITY_SCOPE_CREATE_FUNCTION() {
    return new Scope();
}

EXPORT
void
// cppcheck-suppress unusedFunction
UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base) {
    delete scope_base;
}

}

