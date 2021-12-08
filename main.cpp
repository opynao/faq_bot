#define BOOST_BIND_GLOBAL_PLACEHOLDERS // tgbot

#include <tgbot/tgbot.h>
#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>
#include <tgbot/EventBroadcaster.h>
#include <tgbot/types/CallbackQuery.h>
#include <tgbot/types/InlineQueryResultLocation.h>
#include "utils.h"

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

using spButton = std::shared_ptr<TgBot::InlineKeyboardButton>;
using spMarkup = std::shared_ptr<TgBot::InlineKeyboardMarkup>;
using vButton_t = std::vector<spButton>;
using vButtons_t = std::vector<vButton_t>;

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

spMarkup Eczane()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Где сегодня находятся дежурные аптеки?", "https://www.eczaneler.gen.tr/nobetci-antalya-alanya");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Pcr()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Нужно ли сдавать тест при въезде в РФ гражданам РФ и иностранцам Будет ли штраф если не загрузить результат на ГУ?", 
        "https://zen.yandex.ru/media/id/5ffad3caecead84c12cfc2fd/vopros-nujno-li-sdavat-test-pri-vezde-v-rf-grajdanam-rf-i-inostrancam-budet-li-shtraf-esli-ne-zagruzit-rezultat-na-gu-60e56e1ae34bad584ab89cbe");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Money()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Где находится банкомат, в котором можно снят деньги без комиссии и с кэшбэком?", 
        "https://livetraveling.ru/kak-snjat-dengi-v-turcii-bez-komissii-v-2021-godu/");

    auto button2 = std::make_shared<TgBot::InlineKeyboardButton>("Как зарабатыватьмного денег?", 
        "https://www.ozon.ru/product/razumnyy-investor-polnoe-rukovodstvo-po-stoimostnomu-investirovaniyu-grem-bendzhamin-241464176/?sh=eRhNgH4f");

    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    CreateButtons(button2, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Ikamet()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Как получить икамет?", 
        "https://e-ikamet.goc.gov.tr/Ikamet/Basvuru/IlkBasvuru");

    auto button2 = std::make_shared<TgBot::InlineKeyboardButton>("Как продлить икамет?", 
        "https://e-ikamet.goc.gov.tr/Ikamet/Basvuru/UzatmaBasvuru");

    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    CreateButtons(button2, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Imei()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Как разблокировать IMEI?", 
        "https://turkiye.gov.tr/btk-kullanim-suresi-uzatma");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Developer()
{
    auto button1 = std::make_shared<TgBot::InlineKeyboardButton>("Открыть чат", 
        "https://t.me/opynao");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}
namespace Config
{
    constexpr auto token = "2141352150:AAHzKw-IuAY4zhjehZgYcudhahO1E_5kitY";
}

using userId_t = int64_t;
using chatId_t = int64_t;


class FaqBot {
private:
    TgBot::Bot& m_refTgBot;
    struct BotCommand {
        std::string commandName;
        std::string commandDescription;
        std::function<void(TgBot::Message::Ptr)> commandHandler; 
    };
    std::vector< BotCommand > m_vBotCommands;

    TgBot::Bot& GetTgBot() {
        return m_refTgBot;
    }

    TgBot::Bot const& GetTgBot() const {
        return m_refTgBot;
    }

    auto GetBotCommands() const {
        return m_vBotCommands;
    }

    auto& GetBotCommands() {
        return m_vBotCommands;
    }

    auto& GetTgBotEvents() {
        return GetTgBot().getEvents();
    }

    auto& GetTgBotApi() {
        return GetTgBot().getApi();
    }

    auto GetTgBotApi() const {
        return GetTgBot().getApi();
    }

    void InitBotCommands() {
        auto RegisterCommand = [this] ( const auto& command ) {
            GetTgBotEvents().onCommand( command.commandName, command.commandHandler ); 
        };
        std::ranges::for_each( GetBotCommands(), RegisterCommand );
    }

    void SendMessage( chatId_t chatId, const std::string message ) {
        GetTgBotApi().sendMessage( chatId,  message );
    }

    void SendMessage( chatId_t chatId, const TgBot::GenericReply::Ptr replyMarkup )
    {
        GetTgBotApi().sendMessage( chatId,"Я знаю ответы на следующие вопросы", false, 0, replyMarkup );
    }

    void SendLocation( chatId_t chatId, float latitude, float longitude ) {
        GetTgBotApi().sendLocation( chatId, latitude, longitude );
    }

    static std::string ToMenuItem( const BotCommand& command ) {
        return "/" +  command.commandName + " -- " + command.commandDescription;
    }

    std::string BuildMenu() const
    {
        std::string strMenu;
        for ( auto const& botCommand : GetBotCommands() )
        {
            strMenu.append( "\n" + ToMenuItem( botCommand ) );
        }
        return strMenu;
    }

    void OnCommandStart( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, BuildMenu() );
    }
    
    void OnCommandEczane( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Eczane() );
    }

    void OnCommandPcr( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Pcr() );
    }

    void OnCommandMoney(TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Money() );
    }

    void OnCommandMap( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, "Как попасть слева от чаcов в Махмутларе?" );
        SendLocation( message->chat->id, 36.4885, 32.0962 );
    }

    void OnCommandIkamet( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Ikamet() );
    }

    void OnCommandImei( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Imei() );
    }

    void OnCommandDeveloper( TgBot::Message::Ptr message ) {
        SendMessage( message->chat->id, Developer() );
    }

    void OnCommandHelp( TgBot::Message::Ptr message ) {
        SendMessage(message->chat->id, "Используй /start для того, чтобы начать");
    }

    void OnCommandAddInfo( TgBot::Message::Ptr message )
    {
        SendMessage(message->chat->id, "Добавлено\n");
    }

public:
    explicit FaqBot( TgBot::Bot& bot ) : m_refTgBot ( bot )
        , m_vBotCommands {
            { "start", "Привет! Я знаю ответы на следующие вопросы:", [this]( TgBot::Message::Ptr message ) { OnCommandStart( message ); } },
            { "eczane", "Вся информация об аптеках", [this]( TgBot::Message::Ptr message ) {  OnCommandEczane(  message ); } },
            { "pcr", "Актуальная информация о сдаче ПЦР тестов", [this]( TgBot::Message::Ptr message ) {  OnCommandPcr( message ); } },
            { "money", "Где снять (поменять) деньги", [this]( TgBot::Message::Ptr message ) {  OnCommandMoney( message ); } },
            { "map", "Месторасположение ключевых локаций", [this]( TgBot::Message::Ptr message ) {  OnCommandMap( message ); } },
            { "ikamet", "Информация о получении (продлении) икамета", [this]( TgBot::Message::Ptr message ) { OnCommandIkamet( message ); } },
            { "imei", "Информация о продлении IMEI через госуслуги", [this]( TgBot::Message::Ptr message ) {  OnCommandImei( message ); } },
            { "developer","Сообщить разработчику о проблеме", [this]( TgBot::Message::Ptr message ) {  OnCommandDeveloper( message ); } },
            { "help", "Используй /start для того, чтобы начать", [this]( TgBot::Message::Ptr message ) {  OnCommandHelp( message ); } },
            { "add_info", "Добавить информацию", [this]( TgBot::Message::Ptr message ) {  OnCommandAddInfo( message ); } }
    }{   
        InitBotCommands();
    }
private:

    auto GetChatAdministrators( chatId_t chatId ) {
        return GetTgBotApi().getChatAdministrators( chatId );
    }

    auto IsAdministratorUserId( chatId_t chatId, userId_t userId ) {
        auto IsAdministrator = [] ( userId_t userId ) {
        return [&userId] ( const TgBot::ChatMember::Ptr& ptrChatAdministrator ) {
                return ptrChatAdministrator->user->id == userId;
            };
        };
        return std::ranges::any_of( GetChatAdministrators( chatId ), IsAdministrator( userId ) );
    }
};
 
int main()
{
    TgBot::Bot bot( Config::token );
    FaqBot faqBot( bot );

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