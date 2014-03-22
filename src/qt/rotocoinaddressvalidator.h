#ifndef ROTOCOINADDRESSVALIDATOR_H
#define ROTOCOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator.
   Corrects near-miss characters and refuses characters that are not part of base58.
 */
class RotocoinAddressValidator : public QValidator
{
    Q_OBJECT

public:
    explicit RotocoinAddressValidator(QObject *parent = 0);

    State validate(QString &input, int &pos) const;

    static const int MaxAddressLength = 35;
};

#endif // ROTOCOINADDRESSVALIDATOR_H
