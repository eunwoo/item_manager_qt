#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>
#include <QRegularExpression>

/*
 * https://stackoverflow.com/questions/18676800/how-to-parse-html-with-c-qt
 * https://doc.qt.io/archives/qt-5.5/qtwebkitexamples-webkitwidgets-domtraversal-example.html
*/
class TagElement {
public:
    TagElement(int _start);
    int start;
    int end;
    QString value;
    QStringRef innerValue;
    QString tag;    // html에 출력될 tag
    QString tag_name;   // html에 출력될 tag의 이름만
    QString tag_raw;    // 입력 문자열에 있는 tag
    QString close_tag;

    void MakeInnerValue();
};

enum error_code {
    NO_ERROR = 0,
    OPEN_TAG_NOT_FOUND = -1,
};
class TreeItem {
public:
    TreeItem(TreeItem *_parent);
    TreeItem *parent;
    TagElement *item;
    QVector<TreeItem*> children;
};


class Parser {
public:
    Parser(QString _str);
    ~Parser();
    void Parse();
    bool GetNextTag(int pos);
    void Collapse(int next);

    void ReleaseMemory(TreeItem *item);
    QString toHtml();
    QString toTxt();
    QString TraversalDepthFirst(TreeItem *item, bool generateNoTag = false);

    int pos;
    int parsing_index = 0;
    int tag_open_cnt = 0;
    int error = NO_ERROR;
    QRegularExpression re, re2;
    QString strInput;

    TreeItem *root;
    TreeItem *current;
};

#endif // PARSER_H
