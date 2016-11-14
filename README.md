## Микросервис для выдачи рейтинга игроков

Пример создания простого микросервиса на C++. Коммуникация осуществляется через RabbitMQ.
Использует LevelDB для хранения состояния на случий перезагрузки или подения.
Для сбрасывания рейтинга в конце недели, следует использовать cron + сигнал SIGUSR1.

От ядра системы в микросервис приходят следующие сообщения:
*  *player_registered(id,name)*;
*  *player_renamed(id,name)*;
*  *player_won(id, points)*.

Сервис раз в минуту должен отсылать сообщение с содержимым рейтинга.Рейтинг сортируется по набранным очкам за календарную неделю.

## Обработка сигналов

* SIGUSR1 - сброс рейтинга;
* SIGTERM - остановка сервиса. 

## Используемые проекты

* [AMQP-CPP](https://github.com/CopernicaMarketingSoftware/AMQP-CPP);
* [REACT-CPP](https://github.com/CopernicaMarketingSoftware/REACT-CPP);
* [RapidJSON](https://github.com/miloyip/rapidjson);
* [LevelDB](https://github.com/google/leveldb);
* [Easylogging++](https://github.com/easylogging/easyloggingpp).

## Внешние зависимости

* libc-ares-dev;
* libev-dev.
