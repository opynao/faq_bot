#include "FaqBot.h"

namespace FaqBot {

FaqBot::FaqBot( TgBot::Bot& bot, std::vector<BotCommand> vBotCommands )
        : m_refTgBot ( bot )
        , m_vBotCommands { std::move( vBotCommands ) }
{
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
} // namespace FaqBot