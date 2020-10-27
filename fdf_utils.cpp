#include <iostream>
#include <QDateTime>
#include <QTextCodec>
#include "fdf_utils.h"

#define ERROR_MESSAGE_WAIT_MS (15000)
#define NOF_FIELDS            (68)

typedef unsigned short u16;
typedef unsigned int   u32;
typedef int            i32;
typedef unsigned char  u8;

enum tag_type {
    F_STLV,
    F_BTS,
    F_STR,
    F_INT,
    F_BOOL,
    F_UNXTM,
    F_FDS,
    F_FFD,
    F_UNKWN
};

enum tag {
    TG_AUTO_MODE    = 1001,
    TG_AUTONOMUS    = 1002,
    TG_ADDRESS      = 1009,
    TG_DOC_DATE     = 1012,
    TG_KKT_SN       = 1013,
    TG_TAXID_OFD    = 1017,
    TG_TAXID        = 1018,
    TG_TOTAL        = 1020,
    TG_CASHER       = 1021,
    TG_ITEM_COUNT   = 1023,
    TG_ITEM_NAME    = 1030,
    TG_PAY_CASH     = 1031,
    TG_RN_KKT       = 1037,
    TG_SESSION      = 1038,
    TG_DOC_NUM      = 1040,
    TG_FN_SERIAL    = 1041,
    TG_FD_OF_SESSN  = 1042,
    TG_ITEM_PRICE   = 1043,
    TG_OFD_NAME     = 1046,
    TG_USER_NAME    = 1048,
    TG_FN_TIMEOUT   = 1050,
    TG_FN_REPLACE   = 1051,
    TG_FN_MEM_FULL  = 1052,
    TG_FN_OFD_TMOUT = 1053,
    TG_RECEIPT_TYPE = 1054,
    TG_TAX_SYS      = 1055,
    TG_OFD_SECR     = 1056,
    TG_AGENT        = 1057,
    TG_ITEM         = 1059,
    TG_FTD_URL      = 1060,
    TG_TAXSYSTEMS   = 1062,
    TG_FDS          = 1077,
    TG_ITEM_1PRICE  = 1079,
    TG_PAY_CASHLESS = 1081,
    TG_NOF_NRESP_FD = 1097,
    TG_DATE_1FD_QUE = 1098,
    TG_SUM_20TAX    = 1102,
    TG_SUM_10TAX    = 1103,
    TG_SUM_0TAX     = 1104,
    TG_SUM_NOTAX    = 1105,
    TG_SUM_120TAX   = 1106,
    TG_SUM_110TAX   = 1107,
    TG_KKT_INTERNET = 1108,
    TG_KKT4SERVICE  = 1109,
    TG_AUTO_BSO     = 1110,
    TG_NOF_FD       = 1111,
    TG_OFD_EMAIL    = 1117,
    TG_NOF_FSA      = 1118,
    TG_IS_LOTTERY   = 1126,
    TG_CORR_REASON  = 1174,
    TG_CORR_NAME    = 1177,
    TG_CORR_DATE    = 1178,
    TG_CORR_DOC_NUM = 1179,
    TG_TPLACE       = 1187,
    TG_KKT_VER      = 1188,
    TG_KKT_FFD      = 1189,
    TG_FN_FFD       = 1190,
    TG_IS_CASINO    = 1193,
    TG_TAX          = 1199,
    TG_ITEM_TAX     = 1200,
    TG_IS_EXCISE    = 1207,
    TG_FFD          = 1209,
    TG_ITEM_TYPE    = 1212,
    TG_METHOD       = 1214,
    TG_PAY_ADVANCE  = 1215,
    TG_PAY_CREDIT   = 1216,
    TG_PAY_OTHER    = 1217,
    TG_IN_AUTOMATIC = 1221
};

struct field {
    u16 tag;
    enum tag_type type;
    const char * name;
};

struct field fields[NOF_FIELDS] {
    { TG_AUTO_MODE,    F_BOOL,  "признак автоматического режима" },
    { TG_AUTONOMUS,    F_BOOL,  "признак автономного режима" },
    { TG_ADDRESS,      F_STR,   "адрес расчетов" },
    { TG_DOC_DATE,     F_UNXTM, "дата и время формирования ФД" },
    { TG_KKT_SN,       F_STR,   "заводской номер ККТ" },
    { TG_TAXID_OFD,    F_STR,   "ИНН ОФД" },
    { TG_TAXID,        F_STR,   "ИНН пользователя" },
    { TG_TOTAL,        F_INT,   "сумма расчета, указанного в чеке (БСО)" },
    { TG_CASHER,       F_STR,   "кассир" },
    { TG_ITEM_COUNT,   F_INT,   "количество предмета расчета" },
    { TG_ITEM_NAME,    F_STR,   "наименование предмета расчета" },
    { TG_PAY_CASH,     F_INT,   "сумма по чеку (БСО) наличными" },
    { TG_RN_KKT,       F_STR,   "регистрационный номер ККТ" },
    { TG_SESSION,      F_INT,   "номер смены" },
    { TG_DOC_NUM,      F_INT,   "номер ФД" },
    { TG_FN_SERIAL,    F_STR,   "заводской номер фискального накопителя" },
    { TG_FD_OF_SESSN,  F_INT,   "номер чека за смену" },
    { TG_ITEM_PRICE,   F_INT,   "стоимость предмета расчета с учетом скидок и наценок" },
    { TG_OFD_NAME,     F_STR,   "наименование ОФД" },
    { TG_USER_NAME,    F_STR,   "наименование пользователя" },
    { TG_FN_TIMEOUT,   F_BOOL,  "признак исчерпания ресурса ФН" },
    { TG_FN_REPLACE,   F_BOOL,  "признак необходимости срочной замены ФН" },
    { TG_FN_MEM_FULL,  F_BOOL,  "признак заполнения памяти ФН" },
    { TG_FN_OFD_TMOUT, F_BOOL,  "признак превышения времени ожидания ответа ОФД" },
    { TG_RECEIPT_TYPE, F_INT,   "признак расчета" },
    { TG_TAX_SYS,      F_INT,   "применяемая система налогообложения" },
    { TG_OFD_SECR,     F_BOOL,  "признак передачи ФД ОФД в зашифрованном виде" },
    { TG_AGENT,        F_INT,   "признак агента" },
    { TG_ITEM,         F_STLV,  "предмет расчета" },
    { TG_FTD_URL,      F_STR,   "адрес сайта ФНС" },
    { TG_TAXSYSTEMS,   F_INT,   "системы налогообложения, которые пользователь может применять" },
    { TG_FDS,          F_FDS,   "фискальный признак документа" },
    { TG_ITEM_1PRICE,  F_INT,   "цена за единицу предмета расчета с учетом скидок и наценок" },
    { TG_PAY_CASHLESS, F_INT,   "сумма по чеку (БСО) безналичными" },
    { TG_SUM_20TAX,    F_INT,   "сумма НДС чека по ставке 20%" },
    { TG_SUM_10TAX,    F_INT,   "сумма НДС чека по ставке 10%" },
    { TG_SUM_0TAX,     F_INT,   "сумма НДС чека по ставке 0%" },
    { TG_SUM_NOTAX,    F_INT,   "сумма расчета по чеку без НДС" },
    { TG_SUM_120TAX,   F_INT,   "сумма НДС чека по ставке 20/120%" },
    { TG_SUM_110TAX,   F_INT,   "сумма НДС чека по ставке 10/110%" },
    { TG_KKT_INTERNET, F_BOOL,  "признак ККТ для расчетов только в Интернет" },
    { TG_KKT4SERVICE,  F_BOOL,  "признак применения ККТ при оказании услуг" },
    { TG_AUTO_BSO,     F_BOOL,  "признак ККТ, являющейся автоматизированной системой для БСО" },
    { TG_NOF_FD,       F_INT,   "общее количество ФД за смену" },
    { TG_OFD_EMAIL,    F_STR,   "адрес электронной почты отправителя чека" },
    { TG_NOF_FSA,      F_INT,   "количество кассовых чеков (БСО) за смену" },
    { TG_IS_LOTTERY,   F_BOOL,  "признак проведения лотереи" },
    { TG_CORR_REASON,  F_STLV,  "основание для коррекции" },
    { TG_CORR_NAME,    F_STR,   "описание коррекции" },
    { TG_CORR_DATE,    F_UNXTM, "дата совершения корректируемого расчета" },
    { TG_CORR_DOC_NUM, F_STR,   "номер предписания налогового органа" },
    { TG_TPLACE,       F_STR,   "место расчетов" },
    { TG_KKT_VER,      F_STR,   "версия ККТ" },
    { TG_KKT_FFD,      F_FFD,   "версия ФФД ККТ" },
    { TG_FN_FFD,       F_FFD,   "версия ФФД ФН" },
    { TG_NOF_NRESP_FD, F_INT,   "количество непереданных ФД" },
    { TG_DATE_1FD_QUE, F_UNXTM, "дата первого из непереданных ФД" },
    { TG_IS_CASINO,    F_BOOL,  "признак проведения азартных игр" },
    { TG_TAX,          F_INT,   "ставка НДС" },
    { TG_ITEM_TAX,     F_INT,   "сумма НДС за предмет расчета" },
    { TG_IS_EXCISE,    F_BOOL,  "признак торговли подакцизными товарами" },
    { TG_FFD,          F_FFD,   "номер версии ФФД" },
    { TG_ITEM_TYPE,    F_INT,   "признак предмета расчета" },
    { TG_METHOD,       F_INT,   "признак способа расчета" },
    { TG_PAY_ADVANCE,  F_INT,   "сумма по чеку (БСО) предоплатой" },
    { TG_PAY_CREDIT,   F_INT,   "сумма по чеку (БСО) постоплатой (в кредит)" },
    { TG_PAY_OTHER,    F_INT,   "сумма по чеку (БСО) встречным предоставлением" },
    { TG_IN_AUTOMATIC, F_BOOL,  "признак установки принтера в автомате" },
};

static void _data_format(Ui::MainWindow *ui, QString *text, QByteArray *bts, u32 mux);

static enum tag_type _ftype(u16 tag)
{
    for(u32 i = 0; i < NOF_FIELDS; ++i) {
        if(tag == fields[i].tag)
            return (enum tag_type)fields[i].type;
    }
    return F_UNKWN;
}

static const char * _fname(u16 tag)
{
    for(u32 i = 0; i < NOF_FIELDS; ++i) {
        if(tag == fields[i].tag)
            return fields[i].name;
    }
    return "unknown tag";
}

static u16 _read_u16(QByteArray * bts, u32 off)
{
    u16 tag = (bts->at(off + 0) & 0xFF) | ((bts->at(off + 1) & 0xFF) << 8);
    return tag;
}

static u16 _format_u16(QString * text, QByteArray * bts, u32 off, u32 mux)
{
    char buff[1024];
    u16 val = _read_u16(bts, off);
    sprintf(buff, "%*s%02X %02X ", mux, "", (u8)bts->at(off + 0),
                                            (u8)bts->at(off + 1) );
    text->append(buff);
    return val;
}

static void _tab_print(QString * text, u32 mux)
{
    char buff[64];
    sprintf(buff, "%*s", mux, "");
    text->append(buff);
}

static void _bts_print(QString * text, QByteArray * bts)
{
    char buff[16];
    for (i32 i = 0; i < bts->size(); ++i) {
        sprintf(buff, "%02X ", (u8)bts->at(i));
        text->append(buff);
    }
}

static void _int_comment_print(QString * text, QByteArray * bts) {
    char buff[32];

    if (bts->size() == 1) {
        u8 *num = (u8 *)bts->data();
        sprintf(buff, "(=%d)", *num);
    } else if (bts->size() == 2) {
        u16 *num = (u16 *)bts->data();
        sprintf(buff, "(=%d)", *num);
    } else if (bts->size() == 4) {
        u32 *num = (u32 *)bts->data();
        sprintf(buff, "(=%d)", *num);
    } else {
        sprintf(buff, "(<invalid int|%d|>)", bts->size());
    }

    text->append(buff);
}

static void _str_comment_print(QString * text, QByteArray * bts) {
    char buff[4096];
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    sprintf(buff, "\"%s\"",
                  codec->toUnicode(*bts).toStdString().c_str());
    text->append(buff);
}

static void _unxtm_comment_print(QString * text, QByteArray * bts)
{
    char buff[64];
    QDateTime timestamp;
    u32 * unxtm = (u32 *)bts->data();

    timestamp.setTime_t(*unxtm);
    std::string str = timestamp.toString().toStdString();

    sprintf(buff, "(%s)", str.c_str());
    text->append(buff);
}

static void _fds_comment_print(QString * text, QByteArray * bts) {
    char buff[64];
    u16 * hz  = (u16 *)bts->data();
    u32 * fds = (u32 *)(bts->data() + 2);
    sprintf(buff, "(0x%04X =%u)", *hz, *fds);
    text->append(buff);
}

static const char * _ffd_name(u8 ffd) {
    switch (ffd) {
        case 0x00: return "1.0";
        case 0x01: return "1.05";
        case 0x02: return "1.1";
    default:       return "unknown";
    }
}

static void _ffd_comment_print(QString * text, QByteArray * bts) {
    char buff[64];
    sprintf(buff, "(ФФД %s)", _ffd_name(bts->at(0)));
    text->append(buff);
}

static void _bool_comment_print(QString * text, QByteArray * bts) {
    char buff[16];
    u8 * flag = (u8 *)bts->data();
    sprintf(buff, "\%s", *flag ? "true" : "false");
    text->append(buff);
}

static void _comment_print(QString * text, QByteArray * bts, u16 tag)
{
    text->append("\\\\ ");

    switch (_ftype(tag)) {
        case F_BOOL : _bool_comment_print (text, bts); break;
        case F_INT  : _int_comment_print  (text, bts); break;
        case F_STR  : _str_comment_print  (text, bts); break;
        case F_UNXTM: _unxtm_comment_print(text, bts); break;
        case F_FDS  : _fds_comment_print  (text, bts); break;
        case F_FFD  : _ffd_comment_print  (text, bts); break;
    default:
        break;
    }
    text->append('\n');
}

static void _bytes_format(QString * text, QByteArray * bts, u32 mux, u16 tag)
{
    _tab_print(text, mux);
    _bts_print(text, bts);
    _comment_print(text, bts, tag);
}

static void _format_tag(QString * text, QByteArray * bts, u32 mux)
{
    char buff[1024];
    u16 tag = _format_u16(text, bts, 0, mux);

    sprintf(buff, "\\\\ #%d (%s)\n", tag, _fname(tag));
    text->append(buff);
}

static void _format_len(QString * text, QByteArray * bts, u32 mux)
{
    char buff[64];
    u16 len = _format_u16(text, bts, 2, mux);

    sprintf(buff, "\\\\ (%d)\n", len);
    text->append(buff);
}

static u16 _tag(QByteArray * bts)
{
    return (u8)bts->at(0) | ((u8)bts->at(1) << 8);
}

static u16 _len(QByteArray *bts) {
    if (bts->size() < 4 )
        return 0;

    return (u8)bts->at(2) | ((u8)bts->at(3) << 8);
}

static void _head_format(Ui::MainWindow *ui, QString *text, QByteArray *bts, u32 mux)
{
    u16 tag = _tag(bts);
    QByteArray payload = bts->right(bts->size() - 4);

    _format_tag(text, bts, mux);
    _format_len(text, bts, mux);

    if (_ftype(tag) == F_STLV) _data_format(ui, text, &payload, mux + 6);
    else                       _bytes_format(text, &payload, mux, tag);
}

static void _data_format(Ui::MainWindow *ui, QString *text, QByteArray *bts, u32 mux)
{
    u16 hlen = _len(bts) + 4;

    if ( (hlen == 4) && (bts->size() == 0) ) return;

    QByteArray head = bts->left(hlen);
    QByteArray tail = bts->right(bts->size() - hlen);

    if (head.size() != hlen) {
        ui->statusbar->showMessage("Broken message (incorrect lenght)",
                                   ERROR_MESSAGE_WAIT_MS);
        return;
    }

    _head_format(ui, text, &head, mux);
    _data_format(ui, text, &tail, mux);
}

void fdf_text_formatter(Ui::MainWindow *ui, QString *text)
{
    QByteArray bts = QByteArray::fromHex(text->toUtf8().data());

    text->clear();
    u32 mux = 0;

    _data_format(ui, text, &bts, mux);
}

static inline bool _is_hex(QChar c)
{
    return ('0' <= c && c <= '9') ||
           ('A' <= c && c <= 'F');
}

static void _comment_clear(QString * text) {
    QRegExp exp = QRegExp("\\\\\\\\(.*)\n");
    exp.setMinimal(true);
    text->remove(exp);
}

static void _other_clear(QString * text, QString * buff)
{
    QString copy = text->toUpper();
    for(int i = 0; i < copy.length(); i++)
        if (_is_hex(copy.at(i)))
            buff->append(copy.at(i));
}

static void _split_bytes(QString * buff, QString * text)
{
    text->clear();
    for(int i = 0; i < buff->length(); i++) {
        text->append(buff->at(i));
        text->append(i % 2 ? " " : "");
    }
}

void fdf_clean_src(QString * text)
{
    QString buff;

    text->remove("0x");
    _comment_clear(text);
    _other_clear(text, &buff);
    _split_bytes(&buff, text);
}
