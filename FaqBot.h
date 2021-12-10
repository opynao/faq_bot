#pragma once

#include <tgbot/Bot.h>

#include <memory>
#include <variant>

namespace FaqBot {
class FaqBot;
using chatId_t = int64_t;
using userId_t = int64_t;

struct BotCommand;
class FaqBot final : protected std::enable_shared_from_this<FaqBot> {
public:
    explicit FaqBot( TgBot::Bot& bot, std::vector< BotCommand > botCommands );
    void SendMessage( chatId_t const, std::string const& message );
    void SendMessage( chatId_t const, const TgBot::GenericReply::Ptr );
    void SendLocation( chatId_t const, float const latitude, float const longitude );

private:
    std::vector<TgBot::ChatMember::Ptr> GetChatAdministrators( chatId_t const  chatId ) const;
    bool IsAdministratorUserId( chatId_t const, userId_t const ) const;
    
    TgBot::Bot const& GetTgBot() const;
    TgBot::Bot& GetTgBot();

    TgBot::EventBroadcaster& GetTgBotEvents();
    TgBot::Api const& GetTgBotApi() const;
    
    std::vector< BotCommand > GetBotCommands() const;

    void InitBotCommands();
    void RegisterCommand( BotCommand& );

private:
    TgBot::Bot& m_refTgBot;
    std::vector< BotCommand > m_vBotCommands;
};

struct BotCommand {
public:
    std::string commandName;
    std::string commandDescription;

private:
    std::variant<std::string, std::shared_ptr<TgBot::InlineKeyboardMarkup>> m_Script;
    std::shared_ptr<FaqBot> m_spBot;

private:
    FaqBot& GetFaqBot() {
        return *m_spBot.get();
    }

public:
    BotCommand( std::string name, std::string description
              , std::variant<std::string, std::shared_ptr<TgBot::InlineKeyboardMarkup>> script )
        : commandName { std::move( name ) }
        , commandDescription { std::move( description ) }
        , m_Script { std::move( script ) } {
    }
    ~BotCommand() noexcept = default;

public:
    void SetBot( std::shared_ptr<FaqBot> spBot ) {
        m_spBot = std::move( spBot );
    }

    void operator()( TgBot::Message::Ptr message ) {
        std::visit( [this, &message]( auto&& msg ) {
            GetFaqBot().SendMessage( message->chat->id, msg );
        }, m_Script );
    }
};

} // namespace FaqBot