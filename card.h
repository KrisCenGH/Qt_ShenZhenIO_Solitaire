/*
 * card类：
 * 负责每一张卡牌与卡牌放置区的管理
 */

#ifndef CARD_H
#define CARD_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QSound>

enum {KUANG, FA, ZHONG, FLOWER, BAMBOO, COINS, CHICHAR}; // 七种花色

class Card : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos) // 只使用平移动画

public:
    Card(int suit, int rank, Card *pre = nullptr, Card *next = nullptr, QObject *ob_parent = nullptr, QGraphicsItem *gi_parent = nullptr);
    ~Card();

    int get_suit(); // 获取花色
    int get_rank(); // 获取点数

    void set_suit(int suit); // 设置花色
    void set_rank(int rank); // 设置点数

    Card* get_pre(); // 获取当前卡牌的上一张卡牌对象
    void set_pre(Card *pre_card); // 设置上一张所指向卡牌

    Card* get_next(); // 获取当前卡牌的下一张卡牌对象
    void set_next(Card *next_card); // 设置下一张所指向卡牌

    bool can_press(Card *card); // 判断当前卡牌拿取是否合法

    QRectF boundingRect() const override; // 确定卡牌大小

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override; // 绘制卡牌

    static void area_clear(); // 清空所有卡牌项目

    static int get_completed_card_sum(); // 获取已完成卡牌数量
    static void set_completed_card_sum(int completed_sum); // 设置已完成卡牌数量

    static qreal get_max_zvalue(); // 获取Z值
    static void set_max_zvalue(qreal value); // 设置Z值

    static Card* get_tableau_top(int index); // 获取指定列的Z值最大的卡牌
    static void tableau_push(int index, Card *card); // 向指定列插入纸牌
    static void tableau_pop(int index); // 弹出指定列纸牌

    static Card* get_cell(int index); // 获取cells区卡牌
    static void set_cell(int index, Card *card = nullptr); // 设置cells区卡牌存在情况

    static Card* get_flower(); // 获取flower区卡牌
    static void set_flower(Card *card); // 设置flower区卡牌

    static Card* get_foundation_top(int index); // 获取foundation区Z值最大卡牌
    static void foundation_push(int index, Card *card); // 向指定foundation区插入卡牌
    static void foundation_pop(int index); // 弹出指定foundation区卡牌

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override; // 鼠标相关事件(点击，拖拽，释放)
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int suit; // 卡牌花色，有七种
    int rank; // 卡牌点数，四类特殊牌组(KUANG，FA，ZHONG，FLOWER)点数为-1，其余为1~9
    Card *pre; // 指向上一卡牌
    Card *next; // 指向下一卡牌

    static int completed_card_sum; // 已完成卡牌数量(达到40，即宣告游戏胜利)
    static qreal max_zvalue; // 当前所有存在卡牌的最大Z值

    static Card *flower;
    static QVector<Card*> cells;
    static QVector<QVector<Card*>> tableau;
    static QVector<QVector<Card*>> foundation;

signals:
    void card_change(); // 自定义信号，卡牌每被用户拖动一次，就发送一次此信号并调用一次poke类的check槽函数
};

#endif // CARD_H
