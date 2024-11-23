#include "card.h"

Card::Card(int suit, int rank, Card *pre, Card *next, QObject *ob_parent, QGraphicsItem *gi_parent)
    : QObject(ob_parent)
    , QGraphicsItem(gi_parent)
    , suit(suit)
    , rank(rank)
    , pre(pre)
    , next(next)
{

}

Card::~Card()
{

}

int Card::completed_card_sum = 0;
qreal Card::max_zvalue = 0;

Card* Card::flower = nullptr;
QVector<Card*> Card::cells(3, nullptr);
QVector<QVector<Card*>> Card::tableau(8);
QVector<QVector<Card*>> Card::foundation(3);

int Card::get_suit()
{
    return suit;
}

int Card::get_rank()
{
    return rank;
}

void Card::set_suit(int suit)
{
    this->suit = suit;
}

void Card::set_rank(int rank)
{
    this->rank = rank;
}

Card* Card::get_pre()
{
    return pre;
}

void Card::set_pre(Card *pre_card)
{
    this->pre = pre_card;
}

Card* Card::get_next()
{
    return next;
}

void Card::set_next(Card *next_card)
{
    this->next = next_card;
}

bool Card::can_press(Card *card)
{
    for (Card *ptr = card; ptr; ptr = ptr->next) {
        if (ptr->next && (ptr->suit == ptr->next->suit || ptr->rank - 1 != ptr->next->rank)) {
            return false;
        }
    }
    return true;
}

QRectF Card::boundingRect() const
{
    return QRectF(0, 0, 122, 232); // 前两个坐标参数必须设置为(0, 0)，否则坐标会偏移，原因未知
}

void Card::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) // 此函数包含了各种魔法数字，基本与元素的具体位置有关，请谨慎查看
{
    painter->setPen(Qt::NoPen); // 无边框
    painter->drawRect(boundingRect());

    if (suit != -10) // “哨兵”卡牌不绘制
        painter->drawPixmap(0, 0, QPixmap(":/images/card_front.png"));
    if (suit == -2) { // 已经收归的卡牌，重新绘制为“牌底”
        painter->drawPixmap(0, 0, QPixmap(":/images/card_back.png"));
        return;
    }
    painter->setFont(QFont("Arial", 18, QFont::Bold));

    // 图片可能有某些特性无法被使用的未知问题(libpng warning: iCCP: known incorrect sRGB profile)
    if (suit == KUANG) {
        painter->drawPixmap(5, 3, QPixmap(":/images/small_icons/dragon_white.png"));
        painter->drawPixmap(25, 60, QPixmap(":/images/large_icons/dragon_white.png"));

        painter->translate(116, 228);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap(":/images/small_icons/dragon_white.png"));
    } else if (suit == FA) {
        QPixmap pixmap_small(":/images/small_icons/dragon_green.png"), pixmap_large(":/images/large_icons/dragon_green.png");
        QImage image_small = pixmap_small.toImage(), image_large = pixmap_large.toImage();

        for (int y = 0; y < image_small.height(); ++y) { // 修改图片颜色为绿色
            for (int x = 0; x < image_small.width(); ++x) {
                QColor color = image_small.pixelColor(x, y);
                image_small.setPixelColor(x, y, QColor(0, color.green() * 0.4, 0, color.alpha()));
            }
        }
        for (int y = 0; y < image_large.height(); ++y) {
            for (int x = 0; x < image_large.width(); ++x) {
                QColor color = image_large.pixelColor(x, y);
                image_large.setPixelColor(x, y, QColor(0, color.green() * 0.4, 0, color.alpha()));
            }
        }
        painter->drawPixmap(6, 3, QPixmap::fromImage(image_small));
        painter->drawPixmap(25, 60, QPixmap::fromImage(image_large));

        painter->translate(116, 228);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap::fromImage(image_small));
    } else if (suit == ZHONG) {
        painter->drawPixmap(6, 3, QPixmap(":/images/small_icons/dragon_red.png"));
        painter->drawPixmap(25, 60, QPixmap(":/images/large_icons/dragon_red.png"));

        painter->translate(116, 228);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap(":/images/small_icons/dragon_red.png"));
    } else if (suit == BAMBOO) {
        painter->setPen(QColor(0, 129, 0));
        QPixmap pixmap_small(":/images/small_icons/bamboo.png"), pixmap_large(QString(":/images/large_icons/bamboo_%1.png").arg(rank));
        QImage image_small = pixmap_small.toImage(), image_large = pixmap_large.toImage();

        for (int y = 0; y < image_small.height(); ++y) { // 修改图片颜色为绿色
            for (int x = 0; x < image_small.width(); ++x) {
                QColor color = image_small.pixelColor(x, y);
                image_small.setPixelColor(x, y, QColor(0, color.green() * 0.4, 0, color.alpha()));
            }
        }
        for (int y = 0; y < image_large.height(); ++y) {
            for (int x = 0; x < image_large.width(); ++x) {
                QColor color = image_large.pixelColor(x, y);
                image_large.setPixelColor(x, y, QColor(0, color.green() * 0.4, 0, color.alpha()));
            }
        }
        painter->drawText(8, 24, QString::number(rank));
        painter->drawPixmap(8, 28, QPixmap::fromImage(image_small));
        painter->drawPixmap(25, 60, QPixmap::fromImage(image_large));

        painter->save(); // 保存样式
        painter->translate(116, 210);
        painter->rotate(180);
        painter->drawText(0, 0, QString::number(rank));
        painter->restore(); // 恢复样式
        painter->translate(116, 205);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap::fromImage(image_small));;
    } else if (suit == COINS) {
        painter->setPen(QColor(169, 0, 0));
        painter->drawText(8, 24, QString::number(rank));
        painter->drawPixmap(8, 28, QPixmap(":/images/small_icons/coins.png"));
        painter->drawPixmap(25, 60, QPixmap(QString(":/images/large_icons/coins_%1.png").arg(rank)));

        painter->save(); // 保存样式
        painter->translate(116, 210);
        painter->rotate(180);
        painter->drawText(0, 0, QString::number(rank));
        painter->restore(); // 恢复样式
        painter->translate(116, 205);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap(":/images/small_icons/coins.png"));
    } else if (suit == CHICHAR) {
        painter->setPen(Qt::black);
        painter->drawText(8, 24, QString::number(rank));
        painter->drawPixmap(8, 28, QPixmap(":/images/small_icons/characters.png"));
        painter->drawPixmap(25, 60, QPixmap(QString(":/images/large_icons/char_%1.png").arg(rank)));

        painter->save(); // 保存样式
        painter->translate(116, 210);
        painter->rotate(180);
        painter->drawText(0, 0, QString::number(rank));
        painter->restore(); // 恢复样式
        painter->translate(116, 205);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap(":/images/small_icons/characters.png"));
    } else if (suit == FLOWER) {
        painter->drawPixmap(6, 3, QPixmap(":/images/small_icons/flower.png"));
        painter->drawPixmap(25, 60, QPixmap(":/images/large_icons/flower.png"));

        painter->translate(116, 228);
        painter->rotate(180);
        painter->drawPixmap(0, 0, QPixmap(":/images/small_icons/flower.png"));
    }
}

void Card::area_clear()
{
    completed_card_sum = 0;
    flower = nullptr;
    for (auto &ptr : cells) {
        ptr = nullptr;
    }
    for (auto &arr : tableau) {
        arr.clear();
    }
    for (auto &arr : foundation) {
        arr.clear();
    }
}

int Card::get_completed_card_sum()
{
    return completed_card_sum;
}

void Card::set_completed_card_sum(int completed_sum)
{
    completed_card_sum = completed_sum;
}

qreal Card::get_max_zvalue()
{
    return max_zvalue;
}

void Card::set_max_zvalue(qreal value)
{
    max_zvalue = value;
}

Card* Card::get_tableau_top(int index)
{
    return tableau[index][tableau[index].size() - 1];
}

void Card::tableau_push(int index, Card *card)
{
    tableau[index].push_back(card);
}

void Card::tableau_pop(int index)
{
    tableau[index].pop_back();
}

Card* Card::get_cell(int index)
{
    return cells[index];
}

void Card::set_cell(int index, Card *card)
{
    cells[index] = card;
}

Card* Card::get_flower()
{
    return flower;
}

void Card::set_flower(Card *card)
{
    flower = card;
}

Card* Card::get_foundation_top(int index)
{
    if (foundation[index].isEmpty()) {
        return nullptr;
    }
    return foundation[index][foundation[index].size() - 1];
}

void Card::foundation_push(int index, Card *card)
{
    foundation[index].push_back(card);
}

void Card::foundation_pop(int index)
{
    foundation[index].pop_back();
}

void Card::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (can_press(this)) { // 必须是合法点击
        QSound::play(":/sounds/card_pickup.wav");
        QGraphicsItem::mousePressEvent(event);
    } else {
        event->ignore();
    }
}

void Card::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    for (Card *ptr = this; ptr; ptr = ptr->next) { // 保证一次可以拖拽多张卡牌
        ptr->setZValue(++max_zvalue); // 令被拖拽牌的Z值最大
        ptr->setPos(ptr->pos() + (event->pos() - event->lastPos()));
    }
}

void Card::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) // 此函数亦包含了各种魔法数字，主要与卡牌放置位置相关。因暂未找到更好的位置判定方法，可能会发生放牌不灵敏的情况
{
    if (this->x() > -4 && this->x() < 92 && this->y() > 2 && this->y() < 140 && !this->get_next() && !cells[0]) { // cell[0]区域
        for (auto &items : tableau)
            if (!items.isEmpty() && items[items.size() - 1] == this)
                items.pop_back();
        if (this->get_pre())
            this->get_pre()->set_next(nullptr);
        this->set_pre(nullptr), this->set_next(nullptr);

        if (cells[1] == this) {
            cells[1] = nullptr;
        } else if (cells[2] == this) {
            cells[2] = nullptr;
        }
        cells[0] = this;

        this->setPos(7, 17);
        QSound::play(":/sounds/card_pickup.wav");
    } else if (this->x() > 147 && this->x() < 240 && this->y() > 2 && this->y() < 140 && !this->get_next() && !cells[1]) { // cell[1]区域
        for (auto &items : tableau)
            if (!items.isEmpty() && items[items.size() - 1] == this)
                items.pop_back();
        if (this->get_pre())
            this->get_pre()->set_next(nullptr);
        this->set_pre(nullptr), this->set_next(nullptr);

        if (cells[0] == this) {
            cells[0] = nullptr;
        } else if (cells[2] == this) {
            cells[2] = nullptr;
        }
        cells[1] = this;

        this->setPos(159, 17);
        QSound::play(":/sounds/card_pickup.wav");
    } else if (this->x() > 301 && this->x() < 394 && this->y() > 2 && this->y() < 140 && !this->get_next() && !cells[2]) { // cell[2]区域
        for (auto &items : tableau)
            if (!items.isEmpty() && items[items.size() - 1] == this)
                items.pop_back();
        if (this->get_pre())
            this->get_pre()->set_next(nullptr);
        this->set_pre(nullptr), this->set_next(nullptr);

        if (cells[0] == this) {
            cells[0] = nullptr;
        } else if (cells[1] == this) {
            cells[1] = nullptr;
        }
        cells[2] = this;

        this->setPos(311, 17);
        QSound::play(":/sounds/card_pickup.wav");
    } else if (this->x() > 742 && this->x() < 846 && this->y() > 2 && this->y() < 140 && !this->get_next() && // foundation[0]区域
              ((foundation[0].isEmpty() && this->rank == 1) || (!foundation[0].isEmpty() && foundation[0][foundation[0].size() - 1]->suit == this->suit &&
                                                                                             foundation[0][foundation[0].size() - 1]->rank == this->rank - 1))) {
            for (auto &items : tableau)
                if (!items.isEmpty() && items[items.size() - 1] == this)
                    items.pop_back();
            if (this->get_pre())
                this->get_pre()->set_next(nullptr);
            if (foundation[0].size() > 0) {
                this->set_pre(foundation[0][foundation[0].size() - 1]);
                this->get_pre()->set_next(this);
            }
            foundation[0].push_back(this);

            for (auto &cell : cells) // 来自cells区域的牌需要特殊处理
                if (cell == this)
                    cell = nullptr;

            this->setPos(765, 17);
            QSound::play(":/sounds/card_pickup.wav");
            this->setFlag(QGraphicsItem::ItemIsSelectable, false);
            this->setFlag(QGraphicsItem::ItemIsMovable, false);
            ++completed_card_sum;
    } else if (this->x() > 902 && this->x() < 992 && this->y() > 2 && this->y() < 140 && !this->get_next() && // foundation[1]区域
              ((foundation[1].isEmpty() && this->rank == 1) || (!foundation[1].isEmpty() && foundation[1][foundation[1].size() - 1]->suit == this->suit &&
                                                                                             foundation[1][foundation[1].size() - 1]->rank == this->rank - 1))) {
            for (auto &items : tableau)
                if (!items.isEmpty() && items[items.size() - 1] == this)
                    items.pop_back();
            if (this->get_pre())
                this->get_pre()->set_next(nullptr);
            if (foundation[1].size() > 0) {
                this->set_pre(foundation[1][foundation[1].size() - 1]);
                this->get_pre()->set_next(this);
            }
            foundation[1].push_back(this);

            for (auto &cell : cells) // 来自cells区域的牌需要特殊处理
                if (cell == this)
                    cell = nullptr;

            this->setPos(917, 17);
            QSound::play(":/sounds/card_pickup.wav");
            this->setFlag(QGraphicsItem::ItemIsSelectable, false);
            this->setFlag(QGraphicsItem::ItemIsMovable, false);
            ++completed_card_sum;
    } else if (this->x() > 1054 && this->x() < 1150 && this->y() > 2 && this->y() < 140 && !this->get_next() && // foundation[2]区域
              ( (foundation[2].isEmpty() && this->rank == 1) || (!foundation[2].isEmpty() && foundation[2][foundation[2].size() - 1]->suit == this->suit &&
                                                                                            foundation[2][foundation[2].size() - 1]->rank == this->rank - 1) ) ) {
            for (auto &items : tableau)
                if (!items.isEmpty() && items[items.size() - 1] == this)
                    items.pop_back();
            if (this->get_pre())
                this->get_pre()->set_next(nullptr);
            if (foundation[2].size() > 0) {
                this->set_pre(foundation[2][foundation[2].size() - 1]);
                this->get_pre()->set_next(this);
            }
            foundation[2].push_back(this);

            for (auto &cell : cells) // 来自cells区域的牌需要特殊处理
                if (cell == this)
                    cell = nullptr;

            this->setPos(1069, 17);
            QSound::play(":/sounds/card_pickup.wav");
            this->setFlag(QGraphicsItem::ItemIsSelectable, false);
            this->setFlag(QGraphicsItem::ItemIsMovable, false);
            ++completed_card_sum;
    } else { // 非6个特殊区域(“FLOWER”会自动放置)
        QVector<QGraphicsItem*> items = QVector<QGraphicsItem*>::fromList(this->scene()->items(this->pos()));

        if (items.size() >= 2 && dynamic_cast<Card*>(items[1])->next == nullptr &&
           ((this->rank + 1 == dynamic_cast<Card*>(items[1])->rank && this->suit != dynamic_cast<Card*>(items[1])->suit &&
             dynamic_cast<Card*>(items[1]) != cells[0] && dynamic_cast<Card*>(items[1]) != cells[1] && dynamic_cast<Card*>(items[1]) != cells[2]) || // 避免cells区出现卡牌堆叠情况
                                 (dynamic_cast<Card*>(items[1])->rank == -10 && !dynamic_cast<Card*>(items[1])->get_next()))) { // 合法放置
            Card *top_card = dynamic_cast<Card*>(items[1]);
            if (this->get_pre())
                this->get_pre()->next = nullptr;
            this->set_pre(top_card);
            top_card->next = this;

            // tableau信息更新
            for (int i = 0; i < tableau.size(); ++i)
                for (int j = 0; j < tableau[i].size(); ++j)
                    if (tableau[i][j] == this) {
                        tableau[i].erase(tableau[i].begin() + j, tableau[i].end());
                        break;
                    }

            for (int i = 0; i < tableau.size(); ++i)
                for (int j = 0; j < tableau[i].size(); ++j)
                    if (tableau[i][j] == top_card) {
                        for (Card *ptr = this; ptr; ptr = ptr->next)
                            tableau[i].push_back(ptr);
                        break;
                    }

            for (int i = 0; i < cells.size(); ++i) { // cells区卡牌特殊处理
                if (cells[i] == this)
                    cells[i] = nullptr;
            }
        }

        if (cells[0] == this) { // cells区卡牌特殊处理
            this->setPos(7, 17);
        } else if (cells[1] == this) {
            this->setPos(159, 17);
        } else if (cells[2] == this) {
            this->setPos(311, 17);
        } else {
            for (Card *ptr = this; ptr; ptr = ptr->next) { // 当前选中牌(列)需全部放置
                ptr->setPos(QPointF(ptr->get_pre()->x(), ptr->get_pre()->y() + 26));
            }
            QSound::play(":/sounds/card_place.wav");
        }
    }

    emit card_change(); // 利用信号与槽机制，卡牌每变化一次，就调用一次check函数
}
