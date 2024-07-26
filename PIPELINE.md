The message is picked up from Kafka by MessageConsumer. Then it is put into a message queue.MessageTransformer picks it up from there, performs the necessary IP transformation
and calls AsyncFileManager::writeCapnpMessage, which stores the message in a file.
Every PeriodicSender::INTERVAL, PeriodicSender picks up all the stored messages
from the AsyncFileManager, and then sends them off to DBManager, which constructs SQL insert queries from them.
If the DBManager fails to execute the queries, it returns failure. The PeriodicSender reports back to the file manager whenever
some files are no longer necessary (their contents have been successfully written to the database).
\
TODO indexing for fast search in DB