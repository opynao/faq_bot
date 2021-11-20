#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>
#include <memory>

#include <tgbot/tgbot.h>
#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>
#include "utils.h"

using namespace std;
using namespace TgBot;


int main()
{
    string token("2141352150:AAHzKw-IuAY4zhjehZgYcudhahO1E_5kitY");

    Bot bot(token);

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message)
        {
            auto inlineKeyboardButton1 = std::make_shared<InlineKeyboardButton>("Как получить икамет", "https://yektahomes.ru/question-answer/chto-takoe-ikamet-i-zachem-on-nuzhen/");
            auto inlineKeyboardButton2 = std::make_shared<InlineKeyboardButton>("Как заработать много денег", "https://yektahomes.ru/question-answer/chto-takoe-ikamet-i-zachem-on-nuzhen/");
            std::vector<vector<std::shared_ptr<InlineKeyboardButton>>> vButtons;
            std::vector<std::shared_ptr<InlineKeyboardButton>> vButton;
            vButton.push_back(inlineKeyboardButton1);
            vButtons.push_back(vButton);
            vButton.clear();
            vButton.push_back(inlineKeyboardButton2);
            vButtons.push_back(vButton);

            auto inlineKeyboardMarkup = std::make_shared<InlineKeyboardMarkup>();
            inlineKeyboardMarkup->inlineKeyboard = vButtons;

            bot.getApi().sendMessage(message->chat->id, "Привет! Я знаю ответы на следующие вопросы:", false, message->messageId, inlineKeyboardMarkup);
        });

    bot.getEvents().onCommand("help", [&bot](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Используй /start для того, чтобы начать");
        });


    signal(SIGINT, [](int s)
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