/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ICONITEMDELEGATE_H
#define ICONITEMDELEGATE_H

#include "baseitemdelegate.h"

#include <QStyledItemDelegate>
#include <QTextLayout>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace dfmplugin_workspace {

class IconItemEditor;
class IconItemDelegatePrivate;
class IconItemDelegate : public BaseItemDelegate
{
    Q_OBJECT
    friend class ExpandedItem;

public:
    explicit IconItemDelegate(FileViewHelper *parent);
    ~IconItemDelegate() override;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    bool eventFilter(QObject *object, QEvent *event) override;
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QList<QRect> paintGeomertys(const QStyleOptionViewItem &option, const QModelIndex &index, bool sizeHintMode = false) const override;
    void updateItemSizeHint() override;

    int iconSizeLevel() const override;
    int minimumIconSizeLevel() const override;
    int maximumIconSizeLevel() const override;
    int increaseIcon() override;
    int decreaseIcon() override;
    int setIconSizeByIconSizeLevel(int level) override;

    void hideNotEditingIndexWidget() override;

    QRectF itemIconRect(const QRectF &itemRect) const override;

    QString displayFileName(const QModelIndex &index) const;

private slots:
    void editorFinished();

protected:
    virtual void initTextLayout(const QModelIndex &index, QTextLayout *layout) const override;

private:
    void onTriggerEdit(const QModelIndex &index);

    QPainterPath paintItemBackgroundAndGeomerty(QPainter *painter, const QStyleOptionViewItem &option,
                                                const QModelIndex &index, int backgroundMargin) const;
    QRectF paintItemIcon(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

    void paintItemFileName(QPainter *painter, QRectF iconRect, QPainterPath path, int backgroundMargin, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    QSize iconSizeByIconSizeLevel() const;

    Q_DECLARE_PRIVATE_D(d, IconItemDelegate)
};

}

#endif   // DFMICONITEMDELEGATE_H
