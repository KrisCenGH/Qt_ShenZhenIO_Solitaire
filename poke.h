/*
 * Poke类:
 * 负责总体场景的管理
 * 注：三个QPushButton的Z值暂无法设置到卡牌的下层
 */

#ifndef POKE_H
#define POKE_H

#include "card.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QVector>
#include <QRandomGenerator>
#include <QTimer>
#include <QEventLoop>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class Poke; }
QT_END_NAMESPACE

class Poke : public QMainWindow
{
    Q_OBJECT

public:
    Poke(QWidget *parent = nullptr);
    ~Poke();

private slots:
    void on_action_xinyouxi_triggered();

    void on_action_help_triggered();

    void on_action_guanyu_triggered();

    void on_kuang_pressed();

    void on_fa_pressed();

    void on_zhong_pressed();

private:
    Ui::Poke *ui;
    QGraphicsScene *scene;
    QLabel *help_image; // 帮助

    bool kuang_button_is_press; // 标记按钮按下情况。在新一轮游戏开始前，已经按下的按钮不得再次被按
    bool fa_button_is_press;
    bool zhong_button_is_press;

    void clear(); // 清理场景

    void win_animation(); // 胜利动画播放

    void check(); // 检查三类特殊纸牌(按钮)情况(KUANG, FA, ZHONG)，负责自动收牌以及胜利条件判断

    bool check_less_card(Card *card); // 检查是否有比参数牌点数小1，且花色不同的卡牌

    void set_button_status(QPushButton *button, bool able, bool button_is_press); // 转换按钮状态

    void collect_cards(int suit, Card *auto_collected_card, int auto_collected_area); // 收起三类特殊花色卡牌以及自动收起三类普通花色卡牌(第二、三个参数用于自动收牌。第三个参数是被收的牌的目的位置)
};
#endif // POKE_H
