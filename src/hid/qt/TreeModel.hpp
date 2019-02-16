 #ifndef TREEMODEL_H
 #define TREEMODEL_H

#include <QtCore>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;

class TreeModel : public QAbstractItemModel
{

public:
    TreeModel( const QString &data, QObject * parent = 0 );
    ~TreeModel();

    QVariant         data       ( const QModelIndex &index, int rol ) const;
    Qt::ItemFlags    flags      ( const QModelIndex &index ) const;
    QVariant         headerData ( int section, Qt::Orientation orientation , int role = Qt::DisplayRole ) const;
    QModelIndex      index      ( int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex      parent     ( const QModelIndex &index ) const;
    int              rowCount   ( const QModelIndex &parent = QModelIndex()) const;
    int              columnCount( const QModelIndex &parent = QModelIndex()) const;
    void             resetActive(void);

private:
    TreeItem *rootItem;

};

#endif
