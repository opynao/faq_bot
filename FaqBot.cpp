#include "FaqBot.h"

#include <iostream>
#define LOGF(msg) std::cerr << __FUNCTION__ << ": " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
#define LOGFNM LOGF("")

namespace FaqBot {

FaqBot::FaqBot( TgBot::Bot& bot )
    : m_refTgBot ( bot )
{
}

void FaqBot::Init( Menu_t vBotCommands )
{
    m_vBotCommands = std::move( vBotCommands );
    InitBotCommands();
}

std::vector<TgBot::ChatMember::Ptr> FaqBot::GetChatAdministrators( chatId_t chatId ) const {
    return GetTgBotApi().getChatAdministrators( chatId );
}

bool FaqBot::IsAdministratorUserId( const chatId_t chatId, const userId_t userId ) const {
    auto IsAdministrator = [] ( const userId_t userId ) {
    return [&userId] ( const TgBot::ChatMember::Ptr& ptrChatAdministrator ) {
            return ptrChatAdministrator->user->id == userId;
        };
    };
    return std::ranges::any_of( GetChatAdministrators( chatId ), IsAdministrator( userId ) );
}
    
TgBot::Bot const& FaqBot::GetTgBot() const {
    return m_refTgBot;
}

TgBot::Bot& FaqBot::GetTgBot() {
    return m_refTgBot;
}

TgBot::EventBroadcaster& FaqBot::GetTgBotEvents() {
    return GetTgBot().getEvents();
}

TgBot::Api const& FaqBot::GetTgBotApi() const {
    return GetTgBot().getApi();
}
    
void FaqBot::InitBotCommands() {
    std::ranges::for_each( m_vBotCommands, [this]( BotCommand& command ) {
        RegisterCommand( command );
    });
}

void FaqBot::RegisterCommand( BotCommand& command ) {
    LOGF( command.commandName );
    command.SetBot( shared_from_this() );
    GetTgBotEvents().onCommand( command.commandName, command );
}

void FaqBot::SendMessage( chatId_t const chatId, std::string const& message ) {
    GetTgBotApi().sendMessage( chatId,  message );
}

void FaqBot::SendMessage( chatId_t const chatId, TgBot::GenericReply::Ptr const replyMarkup) {
    GetTgBotApi().sendMessage( chatId,"Я знаю ответы на следующие вопросы", false, 0, replyMarkup );
}

void FaqBot::SendLocation( chatId_t const chatId, float const latitude, float const longitude ) {
    GetTgBotApi().sendLocation( chatId, latitude, longitude );
}

void FaqBot::RegisterMessageHandler( TgBot::EventBroadcaster::MessageListener handler ) {
    GetTgBotEvents().onAnyMessage( handler );
}
} // namespace FaqBot