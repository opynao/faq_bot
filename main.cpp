#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <set>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // tgbot

#include <tgbot/tgbot.h>
#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>
#include <tgbot/EventBroadcaster.h>
#include <tgbot/types/CallbackQuery.h>
#include <tgbot/types/InlineQueryResultLocation.h>
#include "utils.h"

using namespace std;
using namespace TgBot;

using spButton = std::shared_ptr<InlineKeyboardButton>;
using spMarkup = std::shared_ptr<InlineKeyboardMarkup>;
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
    auto markup = std::make_shared<InlineKeyboardMarkup>();
    markup->inlineKeyboard = std::move(vButtons);
    return markup;
}

spMarkup Eczane()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Где сегодня находятся дежурные аптеки?", "https://www.eczaneler.gen.tr/nobetci-antalya-alanya");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Pcr()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Нужно ли сдавать тест при въезде в РФ гражданам РФ и иностранцам Будет ли штраф если не загрузить результат на ГУ?", 
        "https://zen.yandex.ru/media/id/5ffad3caecead84c12cfc2fd/vopros-nujno-li-sdavat-test-pri-vezde-v-rf-grajdanam-rf-i-inostrancam-budet-li-shtraf-esli-ne-zagruzit-rezultat-na-gu-60e56e1ae34bad584ab89cbe");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Money()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Где находится банкомат, в котором можно снят деньги без комиссии и с кэшбэком?", 
        "https://livetraveling.ru/kak-snjat-dengi-v-turcii-bez-komissii-v-2021-godu/");

    auto button2 = std::make_shared<InlineKeyboardButton>("Как зарабатыватьмного денег?", 
        "https://www.ozon.ru/product/razumnyy-investor-polnoe-rukovodstvo-po-stoimostnomu-investirovaniyu-grem-bendzhamin-241464176/?sh=eRhNgH4f");

    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    CreateButtons(button2, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Ikamet()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Как получить икамет?", 
        "https://e-ikamet.goc.gov.tr/Ikamet/Basvuru/IlkBasvuru");

    auto button2 = std::make_shared<InlineKeyboardButton>("Как продлить икамет?", 
        "https://e-ikamet.goc.gov.tr/Ikamet/Basvuru/UzatmaBasvuru");

    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    CreateButtons(button2, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Imei()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Как разблокировать IMEI?", 
        "https://turkiye.gov.tr/btk-kullanim-suresi-uzatma");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}

spMarkup Developer()
{
    auto button1 = std::make_shared<InlineKeyboardButton>("Открыть чат", 
        "https://t.me/opynao");
    vButtons_t vButtons;
    CreateButtons(button1, vButtons);
    return CreateInlineKeyboard(vButtons);
}
namespace Config
{
    constexpr std::int64_t vgUserId = 10490801; 
    const std::set<std::int64_t> vAdministratorsUsersIds = { vgUserId };
    constexpr auto token = "2141352150:AAHzKw-IuAY4zhjehZgYcudhahO1E_5kitY";
}

using userId_t = int64_t;

bool IsAdministratorUserId( userId_t userId )
{
    return Config::vAdministratorsUsersIds.contains( userId );
}

std::string AdminStartMenu()
{
    return "/add_menu - Добавить новое меню\n";
}

std::string BuildMenu ( userId_t userId )
{
    return std::string("Привет! Я знаю ответы на следующие вопросы:\n")
           + "/eczane - Вся информация об аптеках\n"
           + "/pcr - Актуальная информация о сдаче ПЦР тестов\n"
           + "/money - Где сять (поменять) деньги\n"
           + "/map - Месторасположение ключевых локаций\n"
           + "/ikamet - Информация о получении (продлении) икамета\n"
           + "/imei - Информация о продлении IMEI через госуслуги\n"
           + "/developer - Сообщить разработчику о проблеме\n"
           + "/help - Используй /start для того, чтобы начать\n"
           + ( IsAdministratorUserId( userId ) ? AdminStartMenu() : "" );
}

int main()
{
    Bot bot(Config::token);

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
            const std::string menuMessage = BuildMenu( message->from->id );
            bot.getApi().sendMessage(message->chat->id, menuMessage );
    });

    bot.getEvents().onCommand("eczane", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n", false, 0, Eczane());
        });

    bot.getEvents().onCommand("pcr", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n", false, 0, Pcr());
        });

    bot.getEvents().onCommand("money", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n", false, 0, Money());
        });

    bot.getEvents().onCommand("map", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n"
                "Как попасть слева от чаcов в Махмутларе?");
            bot.getApi().sendLocation(message->chat->id, 36.4885, 32.0962);
        });

    bot.getEvents().onCommand("ikamet", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n", false, 0, Ikamet());
        });

    bot.getEvents().onCommand("imei", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Я знаю ответы на следующие опросы:\n", false, 0, Imei());
        });

    bot.getEvents().onCommand("developer", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Связаться с разработчиком\n", false, 0, Developer());
        });

    bot.getEvents().onCommand("help", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Используй /start для того, чтобы начать");
        });

    bot.getEvents().onCommand("add_menu", [&bot](Message::Ptr message)
    {
        if ( IsAdministratorUserId(message->from->id) )
            bot.getApi().sendMessage(message->chat->id, "Добавлено\n");
    });

    signal(SIGINT, [](int)
        {
            printf("SIGINT got\n");
            exit(0);
        });

    try
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true)
        {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e)
    {
        printf("error: %s\n", e.what());
    }
    return 0;
}
