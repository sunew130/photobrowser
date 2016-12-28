#include "qimagelistwidget.h"

QImageListWidget::QImageListWidget(QWidget *parent) :
    QListWidget(parent)
{
    getPos = true;

    this->setStyleSheet(tr("{background: white}"));
    //设置QImageListWidget中的单元项的图片大小
    this->setIconSize(QSize(WICONSIZE,HICONSIZE));
    this->setResizeMode(QListView::Adjust);
    //设置QImageListWidget的显示模式
    this->setViewMode(QListView::IconMode);
    //设置QImageListWidget中的单元项不可被拖动
    this->setMovement(QListView::Static);
    //设置QImageListWidget中的单元项的间距
    this->setSpacing(25);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this,SLOT(selectCurrentItem(QListWidgetItem *, QListWidgetItem *)));

}

QImageListWidget:: ~QImageListWidget()
{

}

//keyevent
void QImageListWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Up)
    {
        if(this->currentRow() <= 3)
        {
            if(this->currentRow() == 0)
                return;

            this->setCurrentRow(this->currentRow() - 1);
            return;
        }

        this->setCurrentRow(this->currentRow() - 4);
    }
    else if(e->key() == Qt::Key_Down)
    {

        if((this->count() - this->currentRow()) <= 4)
        {
            if((this->count() - 1) == this->currentRow())
                return;

            this->setCurrentRow(this->currentRow() + 1);
            return;
        }

        this->setCurrentRow(this->currentRow() + 4);


    }
    else if(e->key() == Qt::Key_Left)
    {
        if(this->currentRow() == 0)
            return;

        this->setCurrentRow(this->currentRow() - 1);
    }
    else if(e->key() == Qt::Key_Right)
    {

        if((this->count() - 1) == this->currentRow())
            return;

        this->setCurrentRow(this->currentRow() + 1);
    }
    else if(e->key() == Qt::Key_Return)
    {
        emit itemClicked(this->currentItem());
    }
    else if(e->key() == Qt::Key_Enter)
    {
        foreach (QString itemname, listItemName) {
            QIconWidget *item = this->findChild<QIconWidget *>(itemname);
            this->removeItemWidget(item->returnItem());
            delete item->returnItem();
            delete item;
        }

        listItemName.clear();
        this->clear();

    }
    else if(e->key() == Qt::Key_O)
    {
        emit clickselectList(this);
    }


    QWidget::keyPressEvent(e);
}


void QImageListWidget::removeListName(QString itemname)
{
    listItemName.removeOne(itemname);
}


void QImageListWidget::addListName(QString itemname)
{
    listItemName<<itemname;
}

void QImageListWidget::changeListName(QString oldnaeme, QString newname)
{
    listItemName.replaceInStrings(oldnaeme,newname);
}


void QImageListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(getPos)
    {
        if(event->pos().y() < 300)
        {
            m =event->pos();
            getPos = false;
        }
        else if(event->pos().y() > 300)
        {
            m =event->pos();
            getPos = false;
        }

    }

    if((m.y() + 20) <= event->pos().y())
    {

        if(this->currentRow()/7 < this->count()/7 - 1)
        {
            this->setCurrentRow(this->currentRow()+7);
        }
        else
        {
            if(this->currentRow() != this->count()-1)
                this->setCurrentRow(this->currentRow()+ 1);
        }
        getPos = true;
    }
    else if((m.y() - 20) >= event->pos().y())
    {
        if((this->count() -this->currentRow())/7 < this->count()/7 - 1)
        {
            this->setCurrentRow(this->currentRow()-7);
        }
        else
        {
            if(this->currentRow() != 0)
                this->setCurrentRow(this->currentRow()- 1);
        }
        getPos = true;
    }

}


void QImageListWidget::selectCurrentItem(QListWidgetItem *current, QListWidgetItem *previous)
{

    if(previous != NULL)
    {
        QIconWidget *pitem = this->findChild<QIconWidget *>(previous->text());
        if(pitem != NULL)
            pitem->setBorderColor("QLabel{background: transparent;}");
    }

    if(current != NULL)
    {
        QIconWidget *citem = this->findChild<QIconWidget *>(current->text());
        if(citem != NULL)
            citem->setBorderColor("QLabel{background: transparent;border-width:5px;border-color:rgb(85, 170, 255);;border-style: solid;border-radius:3}");
    }
}
