#include "preview.h"
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
    ScopeHelper helper;
    sc::Result result = PreviewQueryBase::result();

    if(result["type"].get_string() == "cloudcast"){
        sc::ColumnLayout layoutcol1(1), layoutcol2(2);
        if (result["getExtra"].get_bool()){
            layoutcol1.add_column({"image", "header", "description", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"description", "audio_length", "favoritecount", "listencount", "repostcount", "created"});
        }
        else{
            layoutcol1.add_column({"image", "header", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"audio_length", "favoritecount", "listencount", "repostcount", "created"});
        }
        reply->register_layout({layoutcol1, layoutcol2});
        sc::PreviewWidget created("created", "text"), header("header", "header"), button("button", "actions"), image("image", "image"), expandable("expandable", "expandable"), listencount("listencount", "text"), favoritecount("favoritecount", "text"), repostcount("repostcount", "text"), audio_length("audio_length", "text");
        QString dateString = QString::fromUtf8(result["created"].get_string().data(), result["created"].get_string().size());
        QDate date = QDate::fromString(dateString, Qt::ISODate);
        QString fullDate = date.toString(Qt::SystemLocaleShortDate);
        created.add_attribute_value("text", sc::Variant("Uploaded on " + fullDate.toStdString()));
        header.add_attribute_mapping("title", "title");
        header.add_attribute_mapping("subtitle", "subtitle");
        sc::VariantBuilder builder;
        builder.add_tuple({
            {"id", sc::Variant("open")},
            {"label",sc::Variant("Open")},
            {"uri", result["uri"]}
        });
        image.add_attribute_mapping("source", "art");
        button.add_attribute_value("actions", builder.end());
        expandable.add_attribute_value("title", sc::Variant("Additional info"));
        audio_length.add_attribute_value("text", sc::Variant("⌚ " + result["audio_length"].get_string()));
        listencount.add_attribute_value("text", sc::Variant(std::to_string(result["listen"].get_int()) + " ♪♫"));
        favoritecount.add_attribute_value("text", sc::Variant(std::to_string(result["favorite"].get_int()) + " ♥"));
        repostcount.add_attribute_value("text", sc::Variant(std::to_string(result["repost"].get_int()) + " ♺"));
        expandable.add_widget(audio_length);
        expandable.add_widget(favoritecount);
        expandable.add_widget(listencount);
        expandable.add_widget(repostcount);
        expandable.add_widget(created);
        if (result["getExtra"].get_bool()){
            sc::PreviewWidget description("description", "text");
            description.add_attribute_value("title", sc::Variant("Description"));
            std::vector<std::string> full = helper.getExtra(1, result["username"].get_string() + "/" + result["slug"].get_string());
            if (full[0] != ""){
                description.add_attribute_value("text", sc::Variant(full[0]));
                reply->push({image, header, description, button, expandable, audio_length, favoritecount, listencount, repostcount, created});
            }
            else {
                reply->push({image, header, button, expandable, audio_length, favoritecount, listencount, repostcount, created});
            }
        }
        else{
            reply->push({image, header, button, expandable, audio_length, favoritecount, listencount, repostcount, created});
        }
    }
    else {
        if (result["getExtra"].get_bool()) {
            sc::ColumnLayout layoutcol1(1), layoutcol2(2);
            layoutcol1.add_column({"image", "header", "bio", "expandable", "button"});
            layoutcol2.add_column({"image", "header", "button"});
            layoutcol2.add_column({"bio", "location", "favorite_count", "cloudcast_count", "follower_count", "following_count", "created_time"});
            reply->register_layout({layoutcol1, layoutcol2});
        }
        else {
            sc::ColumnLayout layout(1);
            layout.add_column({"image", "header", "button"});
            reply->register_layout({layout});
        }
        sc::PreviewWidget header("header", "header"), button("button", "actions"), image("image", "image");
        header.add_attribute_mapping("title", "title");
        header.add_attribute_mapping("subtitle", "subtitle");
        sc::VariantBuilder builder;
        builder.add_tuple({
            {"id", sc::Variant("open")},
            {"label",sc::Variant("Open")},
            {"uri", result["uri"]}
        });
        image.add_attribute_mapping("source", "art");
        button.add_attribute_value("actions", builder.end());
        if (result["getExtra"].get_bool()) {
            std::vector<std::string> full = helper.getExtra(2, result["subtitle"].get_string());
            sc::PreviewWidget bio("bio", "text"), expandable("expandable", "expandable"), location("location", "text"), created_time("created_time", "text"), cloudcast_count("cloudcast_count", "text"), favorite_count("favorite_count", "text"), follower_count("follower_count", "text"), following_count("following_count", "text");
            bio.add_attribute_value("title", sc::Variant("Bio"));
            bio.add_attribute_value("text", sc::Variant(full[7]));
            string fullLocation;
            if (full[0] == "" && full[2] == ""){
                fullLocation = "N/A";
            }
            else if (full[0] == "" && full[2] != ""){
                fullLocation = full[2];
            }
            else if (full[0] != "" && full [2] != ""){
                fullLocation = full[0] + ", " + full[2];
            }
            else {
                fullLocation = full[0];
            }
            location.add_attribute_value("text", sc::Variant("Lives in " + fullLocation));
            QString dateString = QString::fromUtf8(full[3].data(), full[3].size());
            QDate date = QDate::fromString(dateString, Qt::ISODate);
            QString fullDate = date.toString(Qt::SystemLocaleShortDate);
            created_time.add_attribute_value("text", sc::Variant("Account created on " + fullDate.toStdString()));
            cloudcast_count.add_attribute_value("text", sc::Variant(full[4] + " ☁"));
            favorite_count.add_attribute_value("text", sc::Variant(full[5] + " ♥"));
            follower_count.add_attribute_value("text", sc::Variant(full[6] + " Followers"));
            following_count.add_attribute_value("text", sc::Variant("Following " + full[1]));
            expandable.add_attribute_value("title", sc::Variant("Additional Info"));
            expandable.add_widget(location);
            expandable.add_widget(favorite_count);
            expandable.add_widget(cloudcast_count);
            expandable.add_widget(follower_count);
            expandable.add_widget(following_count);
            expandable.add_widget(created_time);
            reply->push({image, header, expandable, button, bio, location, favorite_count, cloudcast_count, follower_count, following_count, created_time});
        }
        else{
            reply->push({image, header, button});
        }
    }

}

