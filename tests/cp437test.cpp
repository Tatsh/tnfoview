#include <QtTest/QTest>

#include <cstdlib>

#include "cp437.h"

/** @brief Tests for the CP437 to UTF-8 decoder. */
class Cp437Test : public QObject {
    Q_OBJECT
private Q_SLOTS:
    void emptyInput_returnsEmptyString();
    void ascii_passesThrough();
    void controlPictures_areRemapped();
    void twoByteCharacters_encodeCorrectly();
    void threeByteCharacters_encodeCorrectly();
    void mixedContent_encodesEveryByte();

private:
    /** @brief Decode the bytes and return the result as a byte array, freeing the buffer. */
    static QByteArray decode(const QByteArray &input) {
        char *out = cp437_to_utf8(reinterpret_cast<const unsigned char *>(input.constData()),
                                  static_cast<size_t>(input.size()));
        const QByteArray result(out);
        std::free(out);
        return result;
    }
};

void Cp437Test::emptyInput_returnsEmptyString() {
    QCOMPARE(decode(QByteArray()), QByteArray());
    // A null pointer is allowed only when the length is zero.
    char *out = cp437_to_utf8(nullptr, 0);
    QVERIFY(out != nullptr);
    QCOMPARE(QByteArray(out), QByteArray());
    std::free(out);
}

void Cp437Test::ascii_passesThrough() {
    QCOMPARE(decode(QByteArrayLiteral("Hello, world!")), QByteArrayLiteral("Hello, world!"));
}

void Cp437Test::controlPictures_areRemapped() {
    // CP437 assigns printable code points to a few low byte values rather than the
    // C0 control of the same number, so these are easy to get wrong.
    QCOMPARE(decode(QByteArray(1, '\x1a')), QByteArray("\x1c")); // 0x1A -> U+001C
    QCOMPARE(decode(QByteArray(1, '\x1c')), QByteArray("\x7f")); // 0x1C -> U+007F
    QCOMPARE(decode(QByteArray(1, '\x7f')), QByteArray("\x1a")); // 0x7F -> U+001A
}

void Cp437Test::twoByteCharacters_encodeCorrectly() {
    QCOMPARE(decode(QByteArray(1, '\x80')), QByteArrayLiteral("\xc3\x87")); // Ç U+00C7
    QCOMPARE(decode(QByteArray(1, '\xe1')), QByteArrayLiteral("\xc3\x9f")); // ß U+00DF
    QCOMPARE(decode(QByteArray(1, '\xe0')), QByteArrayLiteral("\xce\xb1")); // α U+03B1
    QCOMPARE(decode(QByteArray(1, '\xff')), QByteArrayLiteral("\xc2\xa0")); // NBSP U+00A0
}

void Cp437Test::threeByteCharacters_encodeCorrectly() {
    QCOMPARE(decode(QByteArray(1, '\xb0')), QByteArrayLiteral("\xe2\x96\x91")); // ░ U+2591
    QCOMPARE(decode(QByteArray(1, '\xdb')), QByteArrayLiteral("\xe2\x96\x88")); // █ U+2588
    QCOMPARE(decode(QByteArray(1, '\xc4')), QByteArrayLiteral("\xe2\x94\x80")); // ─ U+2500
}

void Cp437Test::mixedContent_encodesEveryByte() {
    const QByteArray input =
        QByteArrayLiteral("AB") + QByteArray(1, '\xdb') + QByteArray(1, '\x80');
    QCOMPARE(decode(input), QByteArrayLiteral("AB\xe2\x96\x88\xc3\x87"));
}

QTEST_MAIN(Cp437Test)
#include "cp437test.moc"
