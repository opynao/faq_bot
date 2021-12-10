#define BOOST_BIND_GLOBAL_PLACEHOLDERS // tgbot

#include "FaqBot.h"

#include <tgbot/tgbot.h>
#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>
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

#define LOGF(msg) std::cerr << __FUNCTION__ << ": " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
#define LOGFNM LOGF("")
namespace Config
{
    constexpr auto token = "2141352150:AAHzKw-IuAY4zhjehZgYcudhahO1E_5kitY";
    constexpr auto menuFileName = "menu.json";
}

namespace {

using spButton = std::shared_ptr<TgBot::InlineKeyboardButton>;
using spMarkup = std::shared_ptr<TgBot::InlineKeyboardMarkup>;
using vButton_t = std::vector<spButton>;
using vButtons_t = std::vector<vButton_t>;

std::shared_ptr<TgBot::InlineKeyboardButton> CreateLinkButton( std::string description, std::string url ) {
    return std::make_shared<TgBot::InlineKeyboardButton>( description, url );
}

void CreateButtons(const spButton& button, vButtons_t& vButtons)
{
    LOGFNM;
    vButton_t vButton;
    vButton.push_back(button);
    vButtons.push_back(vButton);
}

spMarkup CreateInlineKeyboard(const vButtons_t& vButtons)
{
    LOGFNM;
    auto markup = std::make_shared<TgBot::InlineKeyboardMarkup>();
    markup->inlineKeyboard = std::move(vButtons);
    return markup;
}

std::string ToMenuItem( FaqBot::BotCommand const& command )
{
    LOGFNM;
    return "/" +  command.commandName + " -- " + command.commandDescription;
}

std::string GetStartMenu( std::vector<FaqBot::BotCommand> const& vBotCommands )
{
    LOGFNM;
    std::string strMenu;
    for ( auto const& botCommand : vBotCommands )
    {
        strMenu.append( "\n" + ToMenuItem( botCommand ) );
    }
    return strMenu;
}

} // namespaces

std::string GetNonEmptyString( rapidjson::Value const& d, std::string name ) {
    LOGFNM;
    if ( !d.HasMember( name.c_str() ) ) {
        throw std::invalid_argument( "document does not have member \"" + name + "\"");
    }
    LOGFNM;
    if( !d[name.c_str()].IsString() ) {
        throw std::invalid_argument( "\"" + name + "\" is not a string");
    }
    LOGFNM;
    if ( !d[name.c_str()].GetStringLength() ) {
        throw std::invalid_argument( "\"" + name + "\" is empty");
    }
    LOGFNM;
    return d[name.c_str()].GetString();
}

template<class T>
auto GetNonEmptyArray( T const& d, std::string const& name ) {
    LOGFNM;
    if ( !d.HasMember( name.c_str() ) ) {
        throw std::invalid_argument( "document does not have member \"" + name + "\"");
    }
    LOGFNM;
    if( !d[name.c_str()].IsArray() ) {
        throw std::invalid_argument( "\"" + name + "\" is not a array");
    }
    LOGFNM;
    if ( d[name.c_str()].Empty() ) {
        throw std::invalid_argument( "\"" + name + "\" is empty");
    }
    LOGFNM;
    return d[name.c_str()].GetArray();
}


int main()
{
    LOGFNM;
    std::vector<FaqBot::BotCommand> vMenu;

    std::ifstream ifsMenu( Config::menuFileName );
    std::string jMenu;

    std::for_each( std::istream_iterator<std::string>( ifsMenu )
             , std::istream_iterator<std::string>()
             , [&jMenu]( auto&& str) { jMenu.append( str ); } );
    
    ifsMenu.close();

    LOGF( "Read:" + jMenu );
    rapidjson::Document d;
    rapidjson::ParseResult ok = d.Parse( jMenu.c_str() );

    assert( d.IsObject() );

    if (  !ok ) {
        throw std::invalid_argument( std::string( Config::menuFileName )
                + ": JSON parse error: " + rapidjson::GetParseError_En( ok.Code() )
                        + " (" + std::to_string( ok.Offset() ) + ")" );
    }

    try {
        LOGFNM;
        auto menus = GetNonEmptyArray( d, "menu" );
        LOGF("Start parcing");
        for( auto const& menu : menus ) {
            std::variant<std::string, std::shared_ptr<TgBot::InlineKeyboardMarkup>> script;
            LOGF("Menu found: " + GetNonEmptyString( menu, "menu_title") );
            if ( menu.HasMember( "actions") ) {
                auto actions = GetNonEmptyArray( menu, "actions" );
                for( auto const& action : actions ) {
                    LOGF("Action found");
                    script = GetNonEmptyString(action, "text");   
                }
            }
            if ( menu.HasMember("buttons") ) {
                auto buttons = GetNonEmptyArray( menu, "buttons" );
                vButtons_t vButtons;
                LOGF("Buttons found");
                for ( auto const& button : buttons ) {
                    LOGF("Button found");
                    auto linkButton = CreateLinkButton( GetNonEmptyString( button["url"], "text" )
                                                   , GetNonEmptyString( button["url"], "link" ) );                    
                    CreateButtons( linkButton, vButtons );
                }
                script = CreateInlineKeyboard(vButtons);
            }
            LOGF("Add menu: " + GetNonEmptyString( menu, "menu_title") );
            vMenu.emplace_back( GetNonEmptyString( menu, "menu_title"), GetNonEmptyString( menu, "menu_description" ), script );
        }
        LOGF("Add main menu");
        vMenu.emplace(vMenu.cbegin(), GetNonEmptyString( d, "menu_title" ), GetNonEmptyString( d, "menu_description" ), GetStartMenu(vMenu ) );

    } catch ( std::invalid_argument const& )
    {
        throw;
    }

    TgBot::Bot bot( Config::token );
    auto spFaqBot = std::make_shared<FaqBot::FaqBot>( bot );
    spFaqBot->Init( vMenu );

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