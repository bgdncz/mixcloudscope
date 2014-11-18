#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>
#include <QDate>

namespace sc = unity::scopes;

using namespace std;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    api::Client client_(Preview::config_);
    sc::Result result = PreviewQueryBase::result();

    if(result["type"].get_string() == "cloudcast"){
        sc::ColumnLayout layoutcol1(1), layoutcol2(2);
        if (result["getExtra"].get_bool()){
            layoutcol1.add_column({"image", "header", "description", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"description", "audio_length", "favoritecount", "listencount", "repostcount"});
        }
        else{
            layoutcol1.add_column({"image", "header", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"audio_length", "favoritecount", "listencount", "repostcount"});
        }
        reply->register_layout({layoutcol1, layoutcol2});
        sc::PreviewWidget header("header", "header");
        header.add_attribute_mapping("title", "title");
        header.add_attribute_mapping("subtitle", "subtitle");
        sc::PreviewWidget button("button", "actions");
        sc::VariantBuilder builder;
        builder.add_tuple({
            {"id", sc::Variant("open")},
            {"label",sc::Variant("Open")},
            {"uri", result["uri"]}
        });
        sc::PreviewWidget image("image", "image");
        image.add_attribute_mapping("source", "art");
        button.add_attribute_value("actions", builder.end());
        sc::PreviewWidget expandable("expandable", "expandable");
        expandable.add_attribute_value("title", sc::Variant("Additional info"));
        sc::PreviewWidget audio_length("audio_length", "text");
        audio_length.add_attribute_value("title", sc::Variant("Length"));
        audio_length.add_attribute_value("text", result["audio_length"]);
        sc::PreviewWidget listencount("listencount", "text");
        sc::PreviewWidget favoritecount("favoritecount", "text");
        sc::PreviewWidget repostcount("repostcount", "text");
        listencount.add_attribute_value("title", sc::Variant("Play Count"));
        favoritecount.add_attribute_value("title", sc::Variant("Favorite Count"));
        repostcount.add_attribute_value("title", sc::Variant("Repost Count"));
        listencount.add_attribute_value("text", (result["listen"].get_int() == 0) ? sc::Variant("None") : result["listen"]);
        favoritecount.add_attribute_value("text", (result["favorite"].get_int() == 0) ? sc::Variant("None") : result["favorite"]);
        repostcount.add_attribute_value("text", (result["repost"].get_int() == 0) ? sc::Variant("None") : result["repost"]);
        expandable.add_widget(audio_length);
        expandable.add_widget(listencount);
        expandable.add_widget(favoritecount);
        expandable.add_widget(repostcount);
        if (result["getExtra"].get_bool()){
            sc::PreviewWidget description("description", "text");
            description.add_attribute_value("title", sc::Variant("Description"));
            std::vector<std::string> full = client_.getExtra(1, result["username"].get_string() + "/" + result["slug"].get_string());
            description.add_attribute_value("text", sc::Variant(full[0]));
            reply->push({image, header, description, button, expandable, audio_length, listencount, favoritecount, repostcount});
        }
        else{
            reply->push({image, header, button, expandable, audio_length, listencount, favoritecount, repostcount});
        }
    }
    else {
        if (result["getExtra"].get_bool()) {
            sc::ColumnLayout layoutcol1(1), layoutcol2(2);
            layoutcol1.add_column({"image", "header", "bio", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"bio", "location", "created_time", "cloudcast_count", "favorite_count", "follower_count", "following_count"});
            reply->register_layout({layoutcol1, layoutcol2});
        }
        else {
            sc::ColumnLayout layout(1);
            layout.add_column({"image", "header", "button"});
            reply->register_layout({layout});
        }
        sc::PreviewWidget header("header", "header");
        header.add_attribute_mapping("title", "title");
        header.add_attribute_mapping("subtitle", "subtitle");
        sc::PreviewWidget button("button", "actions");
        sc::VariantBuilder builder;
        builder.add_tuple({
            {"id", sc::Variant("open")},
            {"label",sc::Variant("Open")},
            {"uri", result["uri"]}
        });
        sc::PreviewWidget image("image", "image");
        image.add_attribute_mapping("source", "art");
        button.add_attribute_value("actions", builder.end());
        if (result["getExtra"].get_bool()) {
            std::vector<std::string> full = client_.getExtra(2, result["subtitle"].get_string());
            sc::PreviewWidget bio("bio", "text"), expandable("expandable", "expandable"), location("location", "text"), created_time("created_time", "text"), cloudcast_count("cloudcast_count", "text"), favorite_count("favorite_count", "text"), follower_count("follower_count", "text"), following_count("following_count", "text");
            bio.add_attribute_value("title", sc::Variant("Bio"));
            bio.add_attribute_value("text", sc::Variant(full[7]));
            std::string fullLocation = full[0] + ", " + full[2];
            //location.add_attribute_value("title", sc::Variant("Location"));
            location.add_attribute_value("text", sc::Variant("Lives in " + fullLocation));
            QString dateString = QString::fromUtf8(full[3].data(), full[3].size());
            QDate date = QDate::fromString(dateString, Qt::ISODate);
            QString fullDate = date.toString("d MMMM yyyy");
            //created_time.add_attribute_value("title", sc::Variant("Created on"));
            created_time.add_attribute_value("text", sc::Variant("Created on " + fullDate.toStdString()));
            //cloudcast_count.add_attribute_value("title", sc::Variant("Cloucast Count"));
            cloudcast_count.add_attribute_value("text", sc::Variant(full[4] + " Cloudcasts"));
            //favorite_count.add_attribute_value("title", sc::Variant("Favorite Count"));
            favorite_count.add_attribute_value("text", sc::Variant(full[5] + " Favorites"));
            //follower_count.add_attribute_value("title", sc::Variant("Follower Count"));
            follower_count.add_attribute_value("text", sc::Variant(full[6] + " Followers"));
            //following_count.add_attribute_value("title", sc::Variant("Following Count"));
            following_count.add_attribute_value("text", sc::Variant("Following " + full[1]));
            expandable.add_attribute_value("title", sc::Variant("Additional Info"));
            expandable.add_widget(created_time);
            expandable.add_widget(location);
            expandable.add_widget(cloudcast_count);
            expandable.add_widget(favorite_count);
            expandable.add_widget(follower_count);
            expandable.add_widget(following_count);
            reply->push({image, header, button, expandable, bio, location, created_time, cloudcast_count, favorite_count, follower_count, following_count});
        }
        else{
            reply->push({image, header, button});
        }
    }

}

