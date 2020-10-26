#include <iostream>
#include <QTextCodec>
#include "fdf_utils.h"

#define ERROR_MESSAGE_WAIT_MS (15000)

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
    F_UNKWN
};

//23 04 \\ #1059 (unknown tag)
//29 00 \\ (41)
//06 04 0B 00 8F E0 AE A4 A0 A6 A0 20 FC 20 33 37 04 01 00 01 FF 03 02 00 03 01 13 04 01 00 00 BE 04 01 00 04 BC 04 01 00 01
//FD 03 \\ #1021 (кассир)
//17 00 \\ (23)
//8A A0 E1 E1 A8 E0 3A 20 92 A5 E1 E2 AE A2 EB A9 20 8A A0 E1 E1 A8 E0 \\ "Кассир: Тестовый Кассир"
//07 04 \\ #1031 (unknown tag)
//01 00 \\ (1)
//00
//39 04 \\ #1081 (unknown tag)
//01 00 \\ (1)
//00
//51 04 \\ #1105 (unknown tag)
//01 00 \\ (1)
//00
//18 04 \\ #1048 (unknown tag)
//0A 00 \\ (10)
//43 69 6E 65 6D 61 50 6C 61 6E
//F1 03 \\ #1009 (unknown tag)
//0D 00 \\ (13)
//47 72 69 73 68 69 6E 61 20 31 35 2F 31
//1F 04 \\ #1055 (unknown tag)
//01 00 \\ (1)
//01
//BF 04 \\ #1215 (unknown tag)
//01 00 \\ (1)
//00
//C0 04 \\ #1216 (unknown tag)
//01 00 \\ (1)
//00
//C1 04 \\ #1217 (unknown tag)
//01 00 \\ (1)
//00
//24 04 \\ #1060 (unknown tag)
//08 00 \\ (8)
//6E 61 6C 6F 67 2E 72 75
//A3 04 \\ #1187 (unknown tag)
//04 00 \\ (4)
//31 33 30 37

enum tag {
    TAG_DOC_DATE     = 1012,
    TAG_TAXID        = 1018,
    TAG_CASHER       = 1021,
    TAG_ITEM_NAME    = 1030,
    TAG_RN_KKT       = 1037,
    TAG_SESSION      = 1038,
    TAG_DOC_NUM      = 1040,
    TAG_FN_SERIAL    = 1041,
    TAG_FD_OF_SESSN  = 1042,
    TAG_FN_TIMEOUT   = 1050,
    TAG_FN_REPLACE   = 1051,
    TAG_FN_MEM_FULL  = 1052,
    TAG_FN_OFD_TMOUT = 1053,
    TAG_METHOD       = 1054,
    TAG_ITEM         = 1059,
    TAG_FDS          = 1077,
    TAG_CORR_REASON  = 1174,
    TAG_CORR_NAME    = 1177,
    TAG_CORR_DATE    = 1178,
    TAG_CORR_DOC_NUM = 1179,
    TAG_KKT_VER      = 1188,
    TAG_KKT_FFD      = 1189,
    TAG_NOF_NRESP_FD = 1097,
    TAG_DATE_1FD_QUE = 1098,
    TAG_NOF_FD       = 1111,
    TAG_NOF_FSA      = 1118,
    TAG_FFD          = 1209,
};

struct field {
    u16 tag;
    enum tag_type type;
    const char * name;
};

struct field fields[27] {
    { TAG_DOC_DATE,     F_UNXTM, "дата и время формирования ФД" },
    { TAG_TAXID,        F_STR,   "ИНН пользователя" },
    { TAG_CASHER,       F_STR,   "кассир" },
    { TAG_ITEM_NAME,    F_STR,   "наименование предмета расчета" },
    { TAG_RN_KKT,       F_STR,   "регистрационный номер ККТ" },
    { TAG_SESSION,      F_INT,   "номер смены" },
    { TAG_DOC_NUM,      F_INT,   "номер ФД" },
    { TAG_FN_SERIAL,    F_STR,   "заводской номер фискального накопителя" },
    { TAG_FD_OF_SESSN,  F_INT,   "номер чека за смену" },
    { TAG_FN_TIMEOUT,   F_BOOL,  "признак исчерпания ресурса ФН" },
    { TAG_FN_REPLACE,   F_BOOL,  "признак необходимости срочной замены ФН" },
    { TAG_FN_MEM_FULL,  F_BOOL,  "признак заполнения памяти ФН" },
    { TAG_FN_OFD_TMOUT, F_BOOL,  "признак превышения времени ожидания ответа ОФД" },
    { TAG_METHOD,       F_INT,   "признак расчета" },
    { TAG_ITEM,         F_STLV,  "предмет расчета" },
    { TAG_FDS,          F_FDS,   "фискальный признак документа" },
    { TAG_CORR_REASON,  F_STLV,  "основание для коррекции" },
    { TAG_CORR_NAME,    F_STR,   "описание коррекции" },
    { TAG_CORR_DATE,    F_UNXTM, "дата совершения корректируемого расчета" },
    { TAG_CORR_DOC_NUM, F_STR,   "номер предписания налогового органа" },
    { TAG_KKT_VER,      F_STR,   "версия ККТ" },
    { TAG_KKT_FFD,      F_INT,   "версия ФФД ККТ" },
    { TAG_NOF_NRESP_FD, F_INT,   "количество непереданных ФД" },
    { TAG_DATE_1FD_QUE, F_UNXTM, "дата первого из непереданных ФД" },
    { TAG_NOF_FD,       F_INT,   "общее количество ФД за смену" },
    { TAG_NOF_FSA,      F_INT,   "количество кассовых чеков (БСО) за смену" },
    { TAG_FFD,          F_INT,   "номер версии ФФД" },
};

static void _data_format(Ui::MainWindow *ui, QString *text, QByteArray *bts, u32 mux);

static enum tag_type _ftype(u16 tag)
{
    for(u32 i = 0; i < sizeof(fields); ++i) {
        if(tag == fields[i].tag)
            return (enum tag_type)fields[i].type;
    }
    return F_UNKWN;
}

static const char * _fname(u16 tag)
{
    for(u32 i = 0; i < sizeof(fields); ++i) {
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
        sprintf(buff, "\\\\ (=%d)", *num);
    } else if (bts->size() == 2) {
        u16 *num = (u16 *)bts->data();
        sprintf(buff, "\\\\ (=%d)", *num);
    } else if (bts->size() == 4) {
        u32 *num = (u32 *)bts->data();
        sprintf(buff, "\\\\ (=%d)", *num);
    } else {
        sprintf(buff, "\\\\ (<invalid int|%d|>)", bts->size());
    }

    text->append(buff);
}

static void _str_comment_print(QString * text, QByteArray * bts) {
    char buff[4096];
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    sprintf(buff, "\\\\ \"%s\"",
                  codec->toUnicode(*bts).toStdString().c_str());
    text->append(buff);
}

static void _unxtm_comment_print(QString * text, QByteArray * bts) {
    char buff[64];
    u32 * unxtm = (u32 *)bts->data();
    sprintf(buff, "\\\\ (%d sec)", *unxtm);
    text->append(buff);
}

static void _fds_print(QString * text, QByteArray * bts) {
    char buff[64];
    u16 * hz  = (u16 *)bts->data();
    u32 * fds = (u32 *)(bts->data() + 2);
    sprintf(buff, "\\\\ (0x%04X =%d)", *hz, *fds);
    text->append(buff);
}

static void _bool_comment_print(QString * text, QByteArray * bts) {
    char buff[16];
    u8 * flag = (u8 *)bts->data();
    sprintf(buff, "\\\\ \%s", *flag ? "true" : "false");
    text->append(buff);
}

static void _comment_print(QString * text, QByteArray * bts, u16 tag)
{
    switch (_ftype(tag)) {
        case F_BOOL : _bool_comment_print (text, bts); break;
        case F_INT  : _int_comment_print  (text, bts); break;
        case F_STR  : _str_comment_print  (text, bts); break;
        case F_UNXTM: _unxtm_comment_print(text, bts); break;
        case F_FDS  : _fds_print          (text, bts); break;
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
