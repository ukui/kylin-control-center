#include "pinyin.h"

#include <QFile>
#include <QTextStream>


static QHash<uint, QString> dict = {};
const char kDictFile[] = ":/dpinyin.dict";

static void InitDict() {
    if (!dict.isEmpty()) {
        return;
    }

    dict.reserve(25333);

    QFile file(kDictFile);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray content = file.readAll();

    file.close();

    QTextStream stream(&content, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        const QStringList items = line.split(QChar(':'));

        if (items.size() == 2) {
            dict.insert(items[0].toInt(nullptr, 16), items[1]);
        }
    }
}

QString Chinese2Pinyin(const QString &words)
{
    InitDict();

    QString result;

    for (int i = 0; i < words.length(); ++i) {
        const uint key = words.at(i).unicode();
        auto find_result = dict.find(key);

        if (find_result != dict.end()) {
            result.append(find_result.value());
        } else {
            result.append(words.at(i));
        }
    }

    return result;
}

