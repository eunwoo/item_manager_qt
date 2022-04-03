#include "parser.h"
#include <QDebug>
#include <QRegularExpression>

Parser::Parser(QString _str)
{
    strInput = _str;
    pos = 0;
}
Parser::~Parser()
{
    ReleaseMemory(root);
}
void Parser::ReleaseMemory(TreeItem *item) {
    for(QVector<TreeItem*>::iterator itr = item->children.begin(); itr<item->children.end(); itr++) {
        ReleaseMemory((TreeItem*)(*itr));
    }
    delete item;
}
void Parser::Parse()
{
    root = new TreeItem(nullptr);
    current = root;
    current->item = new TagElement(0);
    current->item->end = strInput.length();
    current->item->value = strInput.mid(current->item->start, current->item->end);
    re.setPattern("<mb(\\d+)>");
    re2.setPattern("<mt(\\d+)>");

    while(GetNextTag(pos)) {

    }
}
void Parser::Collapse(int next) {
    int pos_close;
    if(next == -1) {
        pos_close = strInput.length();
    }
    else {
        pos_close = strInput.indexOf(">", next + 1) + 1;
    }
    QString close_tag;
    while(current != root) {
        current->item->end = pos_close;
        current->item->value = strInput.mid(current->item->start, current->item->end - current->item->start);
        QRegularExpressionMatch match = re.match(current->item->tag_raw, 0);
        QRegularExpressionMatch match2 = re2.match(current->item->tag_raw, 0);
        if(match.hasMatch()) {
            qInfo() << "found";
            current->item->tag = "<div style=\"margin-bottom:" + match.captured(1) + "rem;\">";
            current->item->tag_name = "<div>";
        }
        else if(match2.hasMatch()) {
            qInfo() << "found";
            current->item->tag = "<div style=\"margin-top:" + match2.captured(1) + "rem;\">";
            current->item->tag_name = "<div>";
        }
        else {
            current->item->tag_raw = strInput.mid(current->item->start, strInput.indexOf(">", current->item->start) + 1 - current->item->start);
            current->item->tag = current->item->tag_raw;
        }
        current->item->MakeInnerValue();
        if(current->item->tag_name == "") {
            close_tag += strInput.mid(current->item->start, strInput.indexOf(">", current->item->start) - current->item->start + 1).insert(1,"/");
        }
        else {
            close_tag += current->item->tag_name.insert(1, "/");
        }
        if(current->parent == root) current->item->close_tag = close_tag;
        current = current->parent;
    }
    current->item->MakeInnerValue();
}
bool Parser::GetNextTag(int pos) {
    int next;

    if((next = strInput.indexOf("<", pos)) == -1) {
        Collapse(next);
        return false;
    }
    else {
        if(strInput.at(next+1) == '/') {
            current->item->end = strInput.indexOf(">", next+1) + 1;
            current->item->value = strInput.mid(current->item->start, current->item->end - current->item->start);
            current->item->MakeInnerValue();
            qInfo() << current->item->innerValue;
            current->item->tag = strInput.mid(current->item->start, strInput.indexOf(">", current->item->start) - current->item->start + 1);
            current = current->parent;
            // Search new child
            return GetNextTag(next+1);
        }
        else if(strInput.at(next+1) == 'x') { // close all open tag
            Collapse(next);
            return GetNextTag(next+1);
        }
        else {
            // Make new child
            TreeItem *child = new TreeItem(current);
            child->item = new TagElement(next); // start는 "<"의 위치를 저장한다.
            int end = strInput.indexOf(">", next);
            child->item->tag_raw = strInput.mid(next, end + 1 - next);
            current->children.push_back(child);

            current = child;
            // Search new child
            return GetNextTag(next+1);
        }
    }
    return true;
}
QString Parser::toHtml()
{
    QString html = TraversalDepthFirst(root);
    html = html.replace("<xx>","");
//    qInfo() << html;
    return html;
}

QString Parser::toTxt()
{
    QString txt = TraversalDepthFirst(root, true);
    return txt;
}

QString Parser::TraversalDepthFirst(TreeItem *item, bool generateNoTag)
{
    QString result;
//    qInfo() << item->item->value;
    int pos = item->item->start;
    if(item->children.length() == 0) {
        if(item == root) {
            result += item->item->innerValue;
        }
    }
    else {
        for(QVector<TreeItem*>::iterator itr = item->children.begin(); itr<item->children.end(); itr++) {

            TreeItem *child = (TreeItem*)(*itr);
            TagElement *e = child->item;
//            qInfo() << strInput.mid(pos, e->start - pos);
            if(strInput.at(pos) != '<') {
                result += strInput.midRef(pos, e->start - pos);
            }
            if(e->tag != "") {
                result += e->tag;
            }
            if(e->innerValue != "") {
                result += e->innerValue;
            }
            result += TraversalDepthFirst(child, generateNoTag);
            if(e->close_tag != "") {
//                qInfo() << e->close_tag;
                result += e->close_tag;
            }
            pos = e->end;
        }
//        qInfo() << strInput.mid(pos, item->item->end - pos);
        result += strInput.midRef(pos, item->item->end - pos);
    }
    return result;
}

TagElement::TagElement(int _start) {
    start = _start;
    end = -1;
    value = "";
}

void TagElement::MakeInnerValue() {
    int open_tag_end = value.indexOf(">");
    int close_tag_start = value.indexOf("<", open_tag_end);
    if(close_tag_start == -1) {
        close_tag_start = value.length();
    }
    innerValue = value.midRef(open_tag_end + 1, close_tag_start - open_tag_end - 1);
}
TreeItem::TreeItem(TreeItem *_parent) {
    parent = _parent;
}
