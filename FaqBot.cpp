#include "FaqBot.h"

#include <iostream>
#define LOGF(msg) std::cerr << __FUNCTION__ << ": " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
#define LOGFNM LOGF("")

namespace FaqBot {

FaqBot::FaqBot( TgBot::Bot& bot )
        : m_refTgBot ( bot )
{
    LOGFNM;
}

void FaqBot::Init( std::vector<BotCommand> vBotCommands )
{
    m_vBotCommands = std::move( vBotCommands );
    InitBotCommands();
}

std::vector<TgBot::ChatMember::Ptr> FaqBot::GetChatAdministrators( chatId_t chatId ) const {
    LOGFNM;
    return GetTgBotApi().getChatAdministrators( chatId );
}

bool FaqBot::IsAdministratorUserId( const chatId_t chatId, const userId_t userId ) const {
    LOGFNM;
    auto IsAdministrator = [] ( const userId_t userId ) {
    return [&userId] ( const TgBot::ChatMember::Ptr& ptrChatAdministrator ) {
            return ptrChatAdministrator->user->id == userId;
        };
    };
    return std::ranges::any_of( GetChatAdministrators( chatId ), IsAdministrator( userId ) );
}
    
TgBot::Bot const& FaqBot::GetTgBot() const {
    LOGFNM;
    return m_refTgBot;
}

TgBot::Bot& FaqBot::GetTgBot() {
    LOGFNM;
    return m_refTgBot;
}

TgBot::EventBroadcaster& FaqBot::GetTgBotEvents() {
    LOGFNM;
    return GetTgBot().getEvents();
}

TgBot::Api const& FaqBot::GetTgBotApi() const {
    LOGFNM;
    return GetTgBot().getApi();
}
    
void FaqBot::InitBotCommands() {
    LOGFNM;
    std::ranges::for_each( m_vBotCommands, [this]( BotCommand& command ) {
        RegisterCommand( command );
    });
}

void FaqBot::RegisterCommand( BotCommand& command ) {
    LOGFNM;
    command.SetBot( shared_from_this() );
    GetTgBotEvents().onCommand( command.commandName, command ); 
}

void FaqBot::SendMessage( chatId_t const chatId, std::string const& message ) {
    LOGFNM;
    GetTgBotApi().sendMessage( chatId,  message );
}

void FaqBot::SendMessage( chatId_t const chatId, TgBot::GenericReply::Ptr const replyMarkup) {
    LOGFNM;
    GetTgBotApi().sendMessage( chatId,"Я знаю ответы на следующие вопросы", false, 0, replyMarkup );
}

void FaqBot::SendLocation( chatId_t const chatId, float const latitude, float const longitude ) {
    LOGFNM;
    GetTgBotApi().sendLocation( chatId, latitude, longitude );
}
} // namespace FaqBot