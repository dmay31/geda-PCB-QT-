#include <QtGui>

#include "TreeModel.hpp"
#include "treeitem.hpp"
#include "data.h"

 TreeModel::TreeModel(const QString &data, QObject *parent)
     : QAbstractItemModel(parent)
{
QList<TreeItem*> parents; //list of parents hierarchy
QList<QVariant> rootData;
QList<QVariant> columnData;
TreeItem * pdata;
rootData << "Project Outline";
rootItem = new TreeItem(rootData);
int x;

parents << rootItem;

columnData << "Layers";

pdata = new TreeItem( columnData , parents.last());
pdata->t = 10;
pdata->icon = new QIcon(":/layers.png");
pdata->layer = 85;
parents.last()->appendChild( pdata );

/* Make Layers new parent */
parents << parents.last()->child(parents.last()->childCount()-1);

/* Fill in each layer */
for( x = 0; x < max_copper_layer; x++ )
    {
    columnData.clear();
    columnData << PCB->Data->Layer[x].Name;
    pdata = new TreeItem( columnData, parents.last());
    QPixmap pixmap( 64,64 );
    pixmap.fill( PCB->Data->Layer[x].Color );
    pdata->icon = new QIcon( pixmap );
    pdata->layer = x;
    pdata->isLayer = TRUE;
    parents.last()->appendChild( pdata );

//    /* Make current layer the new parent */
//    parents << parents.last()->child(parents.last()->childCount()-1);
//    columnData.clear();
//    columnData << "Visible";
//    pdata = new TreeItem( columnData, parents.last());
//    //pdata->icon = new QIcon(":/EyeOn.png");
//    parents.last()->appendChild( pdata );
//
//    columnData.clear();
//    columnData << PCB->Data->Layer[x].Color;
//    pdata = new TreeItem( columnData, parents.last());
//    pdata->icon = new QIcon(":/EyeOn.png");
//    parents.last()->appendChild( pdata );
//
//    /* Pack up to layers level for next layer */
//    parents.pop_back();
    }

}

TreeModel::~TreeModel()
{
delete rootItem;
}

void TreeModel::resetActive(void)
{
int 	  x;
TreeItem* node;


node = rootItem->child(0);


for( x = 0; x < node->childCount(); x++ )
	{
	QModelIndex idx = createIndex( node->row(), 0 );
	node->child(x)->activeLayer = FALSE;

	}
}


QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (!index.isValid())
        return QVariant();


    if (role == Qt::DecorationRole)
        {
        if( item->icon != NULL )
            {
            return *item->icon;
            }
        else if( item->t == 10 )
            {
            return QIcon(":/trace2.png");
            }
        else
            {
            return QIcon(":/via3.png");
            }
        }
    else if( role == Qt::FontRole )
    	{
    	QFont boldFont;
    	if( item->activeLayer )
    		{
    		boldFont.setBold(TRUE);
    		}
    	return( boldFont );
    	}
    else if(role != Qt::DisplayRole)
        {
        return QVariant();
        }


    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
