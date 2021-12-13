#define BOOST_BIND_GLOBAL_PLACEHOLDERS // tgbot

#include "FaqBot.h"

#include <tgbot/tgbot.h>
#include <tgbot/types/KeyboardButton.h>
#include <tgbot/types/ReplyKeyboardMarkup.h>
#include <tgbot/EventBroadcaster.h>
#include <tgbot/types/CallbackQuery.h>
#include <tgbot/types/InlineQueryResultLocation.h>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <set>
#include <functional>
#include <utility>
#include <ranges>
#include <variant>
#include <iostream>
#include <iterator>
#include <numeric>

#define LOGF(msg) std::cerr << __FUNCTION__ << ": " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
#define LOGFNM LOGF("")
namespace Config
{
    constexpr auto token = "2141352150:AAHzKw-IuAY4zhjehZgYcudhahO1E_5kitY";
    constexpr auto menuFileName = "menu.json";
    struct TgApiSupport { constexpr static bool Full = true; };
    constexpr auto alanya_bel = "https://www.alanya.bel.tr/Nobetci-Eczaneler";
    enum class District { ALANYA, MAHMUTLAR, KONAKLI };
}

namespace {

using spMarkup = std::shared_ptr<TgBot::ReplyKeyboardMarkup>;
using vButton_t = std::vector<TgBot::KeyboardButton::Ptr>;
using vButtons_t = std::vector<vButton_t>;

void CreateButtons( TgBot::KeyboardButton::Ptr button, vButtons_t& vButtons) {
    vButton_t vButton;
    vButton.push_back( std::move( button ) );
    vButtons.push_back( vButton );
}



[[deprecated("move to TGBotApiInteface impl")]]
TgBot::ReplyKeyboardMarkup::Ptr CreateInlineKeyboard( vButtons_t vButtons) {
    auto spKbMarkup = std::make_unique<TgBot::ReplyKeyboardMarkup>();
    spKbMarkup->keyboard = std::move( vButtons );
    return spKbMarkup;
}

std::string ToMenuItem( FaqBot::BotCommand const& command ) {
    return "/" +  command.commandName + " -- " + command.commandDescription;
}

std::string GetStartMenu( FaqBot::Menu_t const& vBotCommands ) {
    std::string strMenu;
    for ( auto const & botCommand : vBotCommands )
        strMenu.append( "\n" + ToMenuItem( botCommand ) );
    return strMenu;
}

} // namespaces

std::string GetNonEmptyString( rapidjson::Value const& d, std::string name ) {
    if ( !d.HasMember( name.c_str() ) ) {
        throw std::invalid_argument( "document does not have member \"" + name + "\"");
    }

    if( !d[name.c_str()].IsString() ) {
        throw std::invalid_argument( "\"" + name + "\" is not a string");
    }

    if ( !d[name.c_str()].GetStringLength() ) {
        throw std::invalid_argument( "\"" + name + "\" is empty");
    }

    return d[name.c_str()].GetString();
}

template<class T>
auto GetNonEmptyArray( T const& d, std::string const& name ) {
    if ( !d.HasMember( name.c_str() ) ) {
        throw std::invalid_argument( "document does not have member \"" + name + "\"");
    }

    if( !d[name.c_str()].IsArray() ) {
        throw std::invalid_argument( "\"" + name + "\" is not a array");
    }

    if ( d[name.c_str()].Empty() ) {
        throw std::invalid_argument( "\"" + name + "\" is empty");
    }

    return d[name.c_str()].GetArray();
}

TgBot::KeyboardButton::Ptr GetButton( rapidjson::Value const& button )
{
    if ( !button.HasMember("text") )
        throw std::invalid_argument( "no text value" );

    return std::make_unique<TgBot::KeyboardButton>( button["text"].GetString(),
        button.HasMember( "request_contact" ) && button["request_contact"].IsBool() ? button["request_contact"].GetBool() : false,
        button.HasMember( "request_location" ) && button["request_location"].IsBool() ? button["request_location"].GetBool() : false );
}

class EczaneMessageHandler {
public:
    explicit EczaneMessageHandler( FaqBot::FaqBot& bot ) : m_refBot { bot } {}

    void operator() ( TgBot::Message::Ptr const msg ) {
        if ( msg->location ) {
            auto const pharmacy = GetNearestPharmacyOnDury( msg->location->longitude, msg->location->latitude );
            m_refBot.SendMessage( msg->chat->id, std::get<Name>(pharmacy) );
            m_refBot.SendMessage( msg->chat->id, std::get<Adress>(pharmacy) );
            auto&& [longitude, latitude] = std::>pharmacy.GetLocation();
            m_refBot.SendLocation( msg->chat->id,  latitude, longitude );
        }
    }

    std::pair<float, float> GetLocationOfNearestPharmacy( float longitude, float latitude ) {
        return GetPharmacyOnDutyLocationOfDisrict( DetectDistrict( longitude, latitude ) );
    }

private:
    std::pair<float, float>  GetPharmacyOnDutyLocationOfDisrict( Config::District const  )
    {
        [[deprecated("ONLY FOR TEST")]]
        return { 32.10605800151825, 36.479094763896875 };
    }

    Config::District DetectDistrict( float longitude, float latitude )
    {
        [[deprecated("ONLY FOR TEST")]]
        return Config::District::MAHMUTLAR;
    }

private:
    FaqBot::FaqBot& m_refBot;
};

int main() {


    TgBot::Bot bot( Config::token );
    auto spFaqBot = std::make_shared<FaqBot::FaqBot>( bot );

    std::map<std::string, TgBot::EventBroadcaster::MessageListener > handlers {
        { "eczane", EczaneMessageHandler( *spFaqBot ) }
    };

    FaqBot::Menu_t vMenu;

    std::ifstream ifsMenu( Config::menuFileName );
    std::string jMenu,str;

    LOGF(std::string( "Parse " ) + Config::menuFileName );
    while( std::getline( ifsMenu, str ) ) jMenu.append( str );
    LOGF( jMenu );    
    ifsMenu.close();

    rapidjson::Document d;
    rapidjson::ParseResult ok = d.Parse( jMenu.c_str() );

    if (  !ok ) {
        throw std::invalid_argument( std::string( Config::menuFileName )
                + ": JSON parse error: " + rapidjson::GetParseError_En( ok.Code() )
                        + " (" + std::to_string( ok.Offset() ) + ")" );
    }

    try {
        auto menus = GetNonEmptyArray( d, "menu" );
        for( auto const& menu : menus ) {
            std::variant<std::string, std::shared_ptr<TgBot::ReplyKeyboardMarkup>> script;
            if ( menu.HasMember( "actions") ) {
                auto actions = GetNonEmptyArray( menu, "actions" );
                for( auto const& action : GetNonEmptyArray( menu, "actions" ) ) {
                    script = GetNonEmptyString(action, "text");   
                }
            }

            if ( menu.HasMember("buttons") ) {
                vButtons_t vButtons;
                for ( auto const& button : GetNonEmptyArray( menu, "buttons" ) ) {
                    CreateButtons( GetButton( button ), vButtons );
                }
                script = CreateInlineKeyboard(vButtons);
            }
            vMenu.emplace_back( GetNonEmptyString( menu, "title"), GetNonEmptyString( menu, "description" ), script, handlers[GetNonEmptyString( menu, "title")] );
        }
        vMenu.emplace( vMenu.cbegin(), GetNonEmptyString( d, "title" ), GetNonEmptyString( d, "description" ), GetStartMenu( vMenu ), std::nullopt );

    } catch ( std::invalid_argument const& )
    {
        throw;
    }

    spFaqBot->Init( vMenu );
    [[deprecate("Need auto registering")]]
    spFaqBot->RegisterMessageHandler( handlers["eczane"] );
    signal(SIGINT, [](int)
        {
            printf("SIGINT got\n");
            exit(0);
        });

    try
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(bot);
        while (true)
        {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (std::exception& e)
    {
        printf("error: %s\n", e.what());
    }
    return 0;
}