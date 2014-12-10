 /*
     treeitem.cpp

     A container for items of data supplied by the simple tree model.
 */

 #include <QStringList>

 #include "treeitem.hpp"

 TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
 {
     parentItem = parent;
     itemData = data;
     t = 0;
     isLayer = false;
     activeLayer = false;
 }

 TreeItem::~TreeItem()
 {
     qDeleteAll(childItems);
 }

 void TreeItem::appendChild(TreeItem *item)
 {
     childItems.append(item);
 }

 TreeItem *TreeItem::child(int row)
 {
     return childItems.value(row);
 }

 int TreeItem::childCount() const
 {
     return childItems.count();
 }

 int TreeItem::columnCount() const
 {
     return itemData.count();
 }

 QVariant TreeItem::data(int column) const
 {
     return itemData.value(column);
 }

 TreeItem *TreeItem::parent()
 {
     return parentItem;
 }

 int TreeItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

     return 0;
 }
