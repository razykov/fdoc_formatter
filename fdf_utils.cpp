#include <iostream>
#include "fdf_utils.h"

#define ERROR_MESSAGE_WAIT_MS (15000)

typedef unsigned short u16;
typedef unsigned int   u32;
typedef int            i32;
typedef unsigned char  u8;

enum tag_type {
    F_STLV,
    F_BTS,
    F_UNKWN
};

enum tag {
    TAG_CORR_REASON  = 1174,
    TAG_CORR_NAME    = 1177,
    TAG_CORR_DATE    = 1178,
    TAG_CORR_DOC_NUM = 1179,
};

struct field {
    u16 tag;
    enum tag_type type;
};

struct field fields[4] {
    { TAG_CORR_REASON,  F_STLV },
    { TAG_CORR_NAME,    F_BTS  },
    { TAG_CORR_DATE,    F_BTS  },
    { TAG_CORR_DOC_NUM, F_BTS  },
};

static enum tag_type _ftype(u16 tag) {
    for(u32 i = 0; i < sizeof(fields); ++i) {
        if(tag == fields[i].tag)
            return (enum tag_type)fields[i].type;
    }
    return F_UNKWN;
}

static u16 _read_u16(char * bts) {
    u16 tag = (bts[0] & 0xFF) | ((bts[1] & 0xFF) << 8);
    return tag;
}

static u16 _format_u16(char ** ptr, QString * text, i32 * r, u32 mux) {
    char buff[1024];
    u16 val = _read_u16(*ptr);
    sprintf(buff, "%*s%02X %02X\n", mux, "", (u8)(*ptr)[0], (u8)(*ptr)[1]);
    text->append(buff);
    *ptr += sizeof(u16);
    *r -= sizeof(u16);
    return val;
}

void fdf_text_formatter(Ui::MainWindow *ui, QString * text)
{
    QByteArray bts = QByteArray::fromHex(text->toUtf8().data());
    i32 r = bts.size();
    char * ptr = bts.data();

    text->clear();
    u32 mux = 0;

    while (r > 0) {
        if (r < 4) {
            ui->statusbar->showMessage("Broken message (len < 4)",
                                       ERROR_MESSAGE_WAIT_MS);
            break;
        }

        u16 tag = _format_u16(&ptr, text, &r, mux);
        u16 len = _format_u16(&ptr, text, &r, mux);

        if (r < len) {
            ui->statusbar->showMessage("Broken message (incorrect lenght)",
                                       ERROR_MESSAGE_WAIT_MS);
            break;
        }

        if (_ftype(tag) == F_STLV) {
            mux += 4;
            len = 0;
        } else {
            char buff[4096];
            sprintf(buff, "%*s", mux, "");
            text->append(buff);

            for (u32 i = 0; i < len; ++i) {
                sprintf(buff, "%02X ", (u8)ptr[0]);
                text->append(buff);

                ++ptr;
                --r;
            }

            text->append("\n");
        }
    }
}

static inline bool _is_hex(QChar c) {
    return ('0' <= c && c <= '9') ||
           ('A' <= c && c <= 'F');
}

void fdf_clean_src(QString * text)
{
    QString buff;
    QString copy;

    text->remove("0x");
    copy = text->toUpper();

    for(int i = 0; i < copy.length(); i++)
        if (_is_hex(copy.at(i)))
            buff.append(copy.at(i));

    text->clear();

    for(int i = 0; i < buff.length(); i++) {
        text->append(buff.at(i));
        text->append(i % 2 ? " " : "");
    }
}
