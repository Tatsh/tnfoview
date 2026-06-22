#ifndef CP437_H
#define CP437_H

class QTextCodec;

class QCodePage437Codec : public QTextCodec {
  public:
    QCodePage437Codec();
    ~QCodePage437Codec();

    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;

  protected:
    QString convertToUnicode(const char *in, int length, ConverterState *state) const;
    QByteArray convertFromUnicode(const QChar *in, int length, ConverterState *state) const;
};

#endif // CP437_H
