#pragma once

namespace evt
{
// clang-format off
static const char* treeViewStyleSheet =
R"(
QTreeView, QTreeWidget {
    border: 0px;
    color: <theme.foregroundColor>;
    background-color: <theme.background.normal>;
    alternate-background-color: <theme.background.dark>;
}

QTreeView::item, QTreeWidget::item {
    background: transparent;
    color: <theme.foregroundColor>;
    min-height : 25;
}

QTreeView::item:selected, QTreeView::branch:selected,
QTreeWidget::item:selected, QTreeWidget::branch:selected{
    background-color: <theme.background.light>;
    color: <theme.foregroundColor>;
}

QTreeView::item:hover, QTreeView::branch:hover,
QTreeWidget::item:hover, QTreeWidget::branch:hover {
    background-color: <theme.background.light>;
    color: <theme.foregroundColor>;
}

QHeaderView::section {
    background-color: <theme.background.normal>;
    color: <theme.foregroundColor>;
    min-height : 25;
    border : 0;
    font : bold;
}

QTreeView::branch:has-siblings:!adjoins-item,
QTreeWidget::branch:has-siblings:!adjoins-item{
}

QTreeView::branch:has-siblings:adjoins-item,
QTreeWidget::branch:has-siblings:adjoins-item{
}

QTreeView::branch:!has-children:!has-siblings:adjoins-item,
QTreeWidget::branch:!has-children:!has-siblings:adjoins-item{
}

QTreeView::branch:has-children:!has-siblings:closed,
QTreeView::branch:closed:has-children:has-siblings,
QTreeWidget::branch:has-children:!has-siblings:closed,
QTreeWidget::branch:closed:has-children:has-siblings{
    border-image: none;
    image: url(:/style/icons/arrow_right.png);
}

QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings,
QTreeWidget::branch:open:has-children:!has-siblings,
QTreeWidget::branch:open:has-children:has-siblings{
    border-image: none;
    image: url(:/style/icons/arrow_down.png);
}

)";

static const char* listViewStyleSheet =
R"(
QListView {
    border: 0px;
    color: <theme.foregroundColor>;
    background-color: <theme.background.normal>;
    alternate-background-color: <theme.background.light>;
}
QListView::item {
    background: transparent;
    color: <theme.foregroundColor>;
    min-height : 25;
}
QListView::item:selected,
QListView::branch:selected,
{
    background-color: <theme.background.light>;
    color: <theme.foregroundColor>;
}

QListView::item:hover,
QListView::branch:hover,
{
    background-color: <theme.background.light>;
    color: <theme.foregroundColor>;
}

QHeaderView::section {
    background-color: <theme.background.normal>;
    color: <theme.foregroundColor>;
}

QListView::branch:has-siblings:!adjoins-item{
}

QListView::branch:has-siblings:adjoins-item{
}

QListView::branch:!has-children:!has-siblings:adjoins-item
{
}

QListView::branch:has-children:!has-siblings:closed,
QListView::branch:closed:has-children:has-siblings
{
    border-image: none;
    image: url(:/style/icons/arrow_right.png);
}

QListView::branch:open:has-children:!has-siblings,
QListView::branch:open:has-children:has-siblings
{
    border-image: none;
    image: url(:/style/icons/arrow_down.png);
}

QScrollBar::vertical {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     width: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::horizontal {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     height: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::handle {
     background: <theme.scrollBarHandleColor>;
     min-width: <sizes.scrollBarSize>px;
     min-height: <sizes.scrollBarSize>px;
}

QScrollBar::add-line,  QScrollBar::sub-line {
     background:  <theme.scrollBarBackgroundColor>;
     border: none;
}

QScrollBar::add-page, QScrollBar::sub-page {
     background: <theme.scrollBarBackgroundColor>;
}

)";

// clang-format on
}

