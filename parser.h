#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>

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
    QString tag;
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
    QString strInput;
    void Parse();
    bool GetNextTag(int pos);
    void ReleaseMemory(TreeItem *item);
    QString toHtml();
    QString toTxt();
    QString TraversalDepthFirst(TreeItem *item, bool generateNoTag = false);

    int pos;
    int parsing_index = 0;
    int tag_open_cnt = 0;
    int error = NO_ERROR;

    TreeItem *root;
    TreeItem *current;
};

#endif // PARSER_H
