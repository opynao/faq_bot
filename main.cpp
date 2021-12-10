#define BOOST_BIND_GLOBAL_PLACEHOLDERS // tgbot

#include "FaqBot.h"
#include "utils.h"

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
    vButton_t vButton;
    vButton.push_back(button);
    vButtons.push_back(vButton);
}

spMarkup CreateInlineKeyboard(const vButtons_t& vButtons)
{
    auto markup = std::make_shared<TgBot::InlineKeyboardMarkup>();
    markup->inlineKeyboard = std::move(vButtons);
    return markup;
}

std::string ToMenuItem( FaqBot::BotCommand const& command )
{
    return "/" +  command.commandName + " -- " + command.commandDescription;
}

std::string GetStartMenu( std::vector<FaqBot::BotCommand> const& vBotCommands )
{
    std::string strMenu;
    for ( auto const& botCommand : vBotCommands )
    {
        strMenu.append( "\n" + ToMenuItem( botCommand ) );
    }
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

    if ( d[name.c_str()].Empty() ) {
        throw std::invalid_argument( "\"" + name + "\" is empty");
    }

    return d[name.c_str()].GetString();
}

auto GetNonEmptyArray( rapidjson::Value const& d, std::string name ) {
    
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


int main()
{
    std::vector<FaqBot::BotCommand> vMenu;

    std::ifstream ifsMenu( Config::menuFileName );
    std::string jMenu;
    ifsMenu >> jMenu;
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
            std::variant<std::string, std::shared_ptr<TgBot::InlineKeyboardMarkup>> script;
            if ( menu.HasMember( "actions") ) {
                auto actions = GetNonEmptyArray( menu, "actions" );
                for( auto const& action : actions ) {
                    script = GetNonEmptyString(action, "text");   
                }
            }

            if ( menu.HasMember("url") ) {
                auto urls = GetNonEmptyArray( menu, "url" );
                vButtons_t vButtons;
                for ( auto const& url : urls ) {
                    auto button = CreateLinkButton( GetNonEmptyString(url, "menu_title")
                                                  , GetNonEmptyString( url, "menu_description" ) );                    
                    CreateButtons( button, vButtons );
                }
                script = CreateInlineKeyboard(vButtons);
            }

            vMenu.emplace_back( GetNonEmptyString( menu, "menu_title"), GetNonEmptyString( menu, "menu_description" ), script );
        }
        vMenu.emplace(vMenu.cbegin(), GetNonEmptyString( d, "menu_title" ), GetNonEmptyString( d, "menu_description" ), GetStartMenu(vMenu ) );

    } catch ( std::invalid_argument const& )
    {
        throw;
    }

    TgBot::Bot bot( Config::token );
    auto spFaqBot = std::make_shared<FaqBot::FaqBot>( bot, vMenu );

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