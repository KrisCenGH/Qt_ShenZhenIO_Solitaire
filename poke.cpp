#include "poke.h"
#include "ui_poke.h"

Poke::Poke(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Poke)
    , kuang_button_is_press(false)
    , fa_button_is_press(false)
    , zhong_button_is_press(false)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    help_image = new QLabel(this);

    help_image->setPixmap(QPixmap(":/images/tutorial_large.png"));
    help_image->setGeometry(0, 24, width(), height());
    help_image->hide();

    scene->setSceneRect(0, 0, 1200, 800);
    ui->graphicsView->setScene(scene);

    ui->kuang->setEnabled(false);
    ui->fa->setEnabled(false);
    ui->zhong->setEnabled(false);
}

Poke::~Poke()
{
    delete help_image;
    delete scene;
    delete ui;
}

void Poke::clear()
{
    scene->clear();
    Card::area_clear();

    // 重置按钮状态
    kuang_button_is_press = fa_button_is_press = zhong_button_is_press = false;
    set_button_status(ui->kuang, false, kuang_button_is_press);
    set_button_status(ui->fa, false, fa_button_is_press);
    set_button_status(ui->zhong, false, zhong_button_is_press);
}

void Poke::win_animation()
{
    QVector<QPropertyAnimation*> animation_arr;

    for (int i = 0; i < 9; ++i) {
        if (i < 4) { // cells区
            for (int j = 0; j < 3; ++j) {
                Card *top_card = Card::get_cell(j);
                QPropertyAnimation *animation = new QPropertyAnimation(top_card, "position");

                animation->setStartValue(top_card->pos());
                animation->setEndValue(QPoint(top_card->x(), 850));
                animation->setDuration(1000);
                animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速
                animation_arr.push_back(animation);

                Card::set_cell(j, top_card->get_pre());
            }
        }
        if (i == 0) { // flower区
            Card *top_card = Card::get_flower();
            QPropertyAnimation *animation = new QPropertyAnimation(top_card, "position");

            animation->setStartValue(top_card->pos());
            animation->setEndValue(QPoint(top_card->x(), 850));
            animation->setDuration(1000);
            animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速
            animation_arr.push_back(animation);

            Card::set_flower(nullptr);
        }

        // foundation区
        for (int j = 0; j < 3; ++j) {
            Card *top_card = Card::get_foundation_top(j);
            QPropertyAnimation *animation = new QPropertyAnimation(top_card, "position");

            animation->setStartValue(top_card->pos());
            animation->setEndValue(QPoint(top_card->x(), 850));
            animation->setDuration(1000);
            animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速
            animation_arr.push_back(animation);

            Card::foundation_pop(j);
        }
    }

    QEventLoop loop;
    QTimer *timer = new QTimer(this);

    timer->start(50); // 每隔50ms播放一个新动画

    for (int i = 0; i < animation_arr.size(); ++i) { // 播放动画
        QObject::connect(timer, &QTimer::timeout, &loop, &QEventLoop::quit); // 阻塞当前代码直到达到时间间隔
        animation_arr[i]->start();
        QSound::play(":/sounds/card_deal.wav"); // 播放收牌音效
        loop.exec();

        if (i == animation_arr.size() - 1) { // 阻塞以等待最后一个动画完成
            timer->stop();
            QObject::connect(animation_arr[i], &QPropertyAnimation::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
    }

    for (auto &animation : animation_arr) { // 清理动画资源
        delete animation;
    }
    delete timer;
}

void Poke::check()
{
    repeat_op: // 当卡牌在此函数中变化时，重新执行一遍此函数(goto最有用的一集！！！)

    // 胜利判断
    if (Card::get_completed_card_sum() == 40) {
        win_animation(); // 播放胜利动画
        QMessageBox::information(this, "胜利!", "You Win!!!");
    }

    // 检查是否符合三类特殊卡牌收牌条件(KUANG, FA, ZHONG)
    int sum_KUANG = 0, sum_FA = 0, sum_ZHONG = 0;

    for (int i = 0; i < 8; ++i) {
        if (i < 3) { // cells区域检查
            Card *cell_card = Card::get_cell(i);
            if (cell_card && cell_card->get_suit() == KUANG) {
                ++sum_KUANG;
            } else if (cell_card && cell_card->get_suit() == FA) {
                ++sum_FA;
            } else if (cell_card && cell_card->get_suit() == ZHONG) {
                ++sum_ZHONG;
            }
        }

        Card *top_card = Card::get_tableau_top(i);
        if (top_card->get_suit() == KUANG) {
            ++sum_KUANG;
        } else if (top_card->get_suit() == FA) {
            ++sum_FA;
        } else if (top_card->get_suit() == ZHONG) {
            ++sum_ZHONG;
        } else if (top_card->get_suit() == FLOWER) { // 将“FLOWER”收归
            top_card->setFlag(QGraphicsItem::ItemIsSelectable, false);
            top_card->setFlag(QGraphicsItem::ItemIsMovable, false);

            QPropertyAnimation *animation = new QPropertyAnimation(top_card, "position");

            animation->setStartValue(top_card->pos());
            animation->setEndValue(QPoint(575, 17));
            animation->setDuration(300);
            animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速

            QEventLoop loop;
            QObject::connect(animation, &QPropertyAnimation::finished, &loop, &QEventLoop::quit); // 阻塞当前代码直到动画完成
            animation->start();
            QSound::play(":/sounds/card_sweep.wav"); // 播放收牌音效
            loop.exec();
            delete animation;

            if (top_card->get_pre())
                top_card->get_pre()->set_next(nullptr);
            top_card->set_next(nullptr);
            Card::tableau_pop(i);
            Card::set_flower(top_card);

            Card::set_completed_card_sum(Card::get_completed_card_sum() + 1);

            goto repeat_op; // 卡牌变化，重新执行此函数
        }
    }

    if (sum_KUANG == 4 && ( (Card::get_cell(0) && Card::get_cell(0)->get_suit() == KUANG) ||
                            (Card::get_cell(1) && Card::get_cell(1)->get_suit() == KUANG) ||
                            (Card::get_cell(2) && Card::get_cell(2)->get_suit() == KUANG) ||
                             !Card::get_cell(0) || !Card::get_cell(1) || !Card::get_cell(2) )) {
        set_button_status(ui->kuang, true, kuang_button_is_press);
    } else {
        set_button_status(ui->kuang, false, kuang_button_is_press);
    }
    if (sum_FA == 4 && ( (Card::get_cell(0) && Card::get_cell(0)->get_suit() == FA) ||
                         (Card::get_cell(1) && Card::get_cell(1)->get_suit() == FA) ||
                         (Card::get_cell(2) && Card::get_cell(2)->get_suit() == FA) ||
                          !Card::get_cell(0) || !Card::get_cell(1) || !Card::get_cell(2) )) {
        set_button_status(ui->fa, true, fa_button_is_press);
    } else {
        set_button_status(ui->fa, false, fa_button_is_press);
    }
    if (sum_ZHONG == 4 && ( (Card::get_cell(0) && Card::get_cell(0)->get_suit() == ZHONG) ||
                            (Card::get_cell(1) && Card::get_cell(1)->get_suit() == ZHONG) ||
                            (Card::get_cell(2) && Card::get_cell(2)->get_suit() == ZHONG) ||
                             !Card::get_cell(0) || !Card::get_cell(1) || !Card::get_cell(2) )) {
        set_button_status(ui->zhong, true, zhong_button_is_press);
    } else {
        set_button_status(ui->zhong, false, zhong_button_is_press);
    }

    // 自动收牌
    for (int i = 0; i < 3; ++i) {
        Card *found_card = Card::get_foundation_top(i);

        for (int j = 0; j < 3; ++j) { // cells区判断
            Card *cell_card = Card::get_cell(j);

            if (cell_card && ( (!found_card && cell_card->get_rank() == 1) ||
                             (found_card && cell_card->get_suit() == found_card->get_suit() && cell_card->get_rank() - 1 == found_card->get_rank() &&
                              !check_less_card(cell_card)) ) ) {
                collect_cards(cell_card->get_suit(), cell_card, i);
                goto repeat_op; // 卡牌布局变化，重新检查
            }
        }

        for (int j = 0; j < 8; ++j) {
            Card *table_card = Card::get_tableau_top(j);

            if (table_card && ( (!found_card && table_card->get_rank() == 1) ||
                              (found_card && table_card->get_suit() == found_card->get_suit() && table_card->get_rank() - 1 == found_card->get_rank() &&
                                !check_less_card(table_card)) ) ) {
                collect_cards(table_card->get_suit(), table_card, i);
                goto repeat_op; // 卡牌布局变化，重新检查
            }
        }
    }
}

bool Poke::check_less_card(Card *card)
{
    for (int i = 0; i < 3; ++i) { // 检查cells区
        Card *cell_card = Card::get_cell(i);

        if (cell_card && card->get_suit() == cell_card->get_suit() && card->get_rank() - 1 == cell_card->get_rank())
            return true;
    }

    for (int i = 0; i < 8; ++i) { // 检查tableau区
        Card *table_card = Card::get_tableau_top(i);

        if (table_card && card->get_suit() == table_card->get_suit() && card->get_rank() - 1 == table_card->get_rank())
            return true;
    }

    return false;
}

void Poke::set_button_status(QPushButton *button, bool able, bool button_is_press)
{
    if (!button_is_press) { // 未被按下，可以转换状态
        if (able) {
            button->setEnabled(true);
            if (button->objectName() == "kuang") {
                button->setStyleSheet( // 当四张“kuang”全部可以被收归至左边牌区时，改变按钮样式
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_white_active.png);"
                            );
            } else if (button->objectName() == "fa") {
                ui->fa->setStyleSheet( // 当四张“fa”全部可以被收归至左边牌区时，改变按钮样式
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_green_active.png);"
                            );
            } else if (button->objectName() == "zhong") {
                ui->zhong->setStyleSheet( // 当四张“zhong”全部可以被收归至左边牌区时，改变按钮样式
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_red_active.png);"
                            );
            }
        } else {
            button->setEnabled(false);
            if (button->objectName() == "kuang") {
                button->setStyleSheet(
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_white_up.png);"
                            );
            } else if (button->objectName() == "fa") {
                ui->fa->setStyleSheet(
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_green_up.png);"
                            );
            } else if (button->objectName() == "zhong") {
                ui->zhong->setStyleSheet(
                            "border-radius: 36px;"
                            "color: transparent;"
                            "background-image: url(:/images/button_red_up.png);"
                            );
            }
        }
    }
}

void Poke::collect_cards(int suit, Card *auto_collected_card, int auto_collected_area)
{
    if (auto_collected_card) { // 自动收起三种普通花色牌
        QPointF target_pos(0.0, 0.0); // 确定收牌位置
        if (auto_collected_area == 0) {
            target_pos = {765, 17};
        } else if (auto_collected_area == 1) {
            target_pos = {917, 17};
        } else if (auto_collected_area == 2) {
            target_pos = {1069, 17};
        }

        auto_collected_card->setFlag(QGraphicsItem::ItemIsSelectable, false);
        auto_collected_card->setFlag(QGraphicsItem::ItemIsMovable, false);

        if (auto_collected_card->get_pre())
            auto_collected_card->get_pre()->set_next(nullptr);
        auto_collected_card->set_pre(Card::get_foundation_top(auto_collected_area));
        if (Card::get_foundation_top(auto_collected_area))
            Card::get_foundation_top(auto_collected_area)->set_next(auto_collected_card);

        Card::foundation_push(auto_collected_area, auto_collected_card);
        for (int i = 0; i < 3; ++i) {
            if (Card::get_cell(i) == auto_collected_card) {
                Card::set_cell(i);
            }
        }
        for (int i = 0; i < 8; ++i) {
            if (Card::get_tableau_top(i) == auto_collected_card) {
                Card::tableau_pop(i);
            }
        }

        Card::set_max_zvalue(Card::get_max_zvalue() + 1);
        auto_collected_card->setZValue(Card::get_max_zvalue());

        QPropertyAnimation *animation = new QPropertyAnimation(auto_collected_card, "position");

        animation->setStartValue(auto_collected_card->pos());
        animation->setEndValue(QPoint(target_pos.x(), target_pos.y()));
        animation->setDuration(400);
        animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速

        QEventLoop loop;
        QObject::connect(animation, &QPropertyAnimation::finished, &loop, &QEventLoop::quit); // 阻塞当前代码直到动画完成
        animation->start();
        QSound::play(":/sounds/card_sweep.wav"); // 播放收牌音效
        loop.exec();

        Card::set_completed_card_sum(Card::get_completed_card_sum() + 1);

        delete animation;
    } else { // 收起三种特殊花色牌
        int collect_area = -1; // 准备确定收牌位置

        for (int i = 0; i < 3 && collect_area == -1; ++i) { // 优先放到有“suit”的cell区
            if (Card::get_cell(i) && Card::get_cell(i)->get_suit() == suit) {
                collect_area = i;
            }
        }
        for (int i = 0; i < 3 && collect_area == -1; ++i) { // 放到第一个空置区域
            if (!Card::get_cell(i)) {
                collect_area = i;
            }
        }

        QPointF target_pos(0.0, 0.0);
        if (collect_area == 0) {
            target_pos = {7, 17};
        } else if (collect_area == 1) {
            target_pos = {159, 17};
        } else if (collect_area == 2) {
            target_pos = {311, 17};
        }

        QParallelAnimationGroup *para_group = new QParallelAnimationGroup;
        QVector<Card*> arr;

        for (int i = 0; i < 3; ++i) {
            Card *card = Card::get_cell(i);
            if (card && card->get_suit() == suit) {
                card->setFlag(QGraphicsItem::ItemIsSelectable, false);
                card->setFlag(QGraphicsItem::ItemIsMovable, false);

                // 收归，重新绘制
                card->set_suit(-2);
                card->update();

                QPropertyAnimation *animation = new QPropertyAnimation(card, "position");

                animation->setStartValue(card->pos());
                animation->setEndValue(QPoint(target_pos.x(), target_pos.y()));
                animation->setDuration(300);
                animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速

                para_group->addAnimation(animation);

                if (card->pos() != target_pos) {
                    Card::set_cell(i);
                }

                arr.push_back(card);
                if (arr.size() > 1) {
                    arr[arr.size() - 1]->set_pre(arr[arr.size() - 2]);
                    arr[arr.size() - 2]->set_next(arr[arr.size() - 1]);
                }
            }
        }
        for (int i = 0; i < 8; ++i) {
            Card *card = Card::get_tableau_top(i);
            if (card && card->get_suit() == suit) {
                card->setFlag(QGraphicsItem::ItemIsSelectable, false);
                card->setFlag(QGraphicsItem::ItemIsMovable, false);

                // 收归，重新绘制
                card->set_suit(-2);
                card->update();

                QPropertyAnimation *animation = new QPropertyAnimation(card, "position");

                animation->setStartValue(card->pos());
                animation->setEndValue(QPoint(target_pos.x(), target_pos.y()));
                animation->setDuration(300);
                animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速

                para_group->addAnimation(animation);

                if (card->get_pre())
                    card->get_pre()->set_next(nullptr);
                Card::tableau_pop(i);

                arr.push_back(card);
                if (arr.size() > 1) {
                    arr[arr.size() - 1]->set_pre(arr[arr.size() - 2]);
                    arr[arr.size() - 2]->set_next(arr[arr.size() - 1]);
                }
            }
        }

        QEventLoop loop;
        QObject::connect(para_group, &QParallelAnimationGroup::finished, &loop, &QEventLoop::quit); // 阻塞当前代码直到动画完成
        para_group->start();
        QSound::play(":/sounds/card_sweep.wav"); // 播放收牌音效
        loop.exec();

        Card::set_cell(collect_area, arr[arr.size() - 1]);

        para_group->clear();
        delete para_group;

        // 按钮已被按下，不可转换状态(重新开始游戏除外)
        if (suit == KUANG) {
            kuang_button_is_press = true;
        } else if (suit == FA) {
            fa_button_is_press = true;
        } else if (suit == ZHONG) {
            zhong_button_is_press = true;
        }

        Card::set_completed_card_sum(Card::get_completed_card_sum() + 4);

        check(); // 卡牌布局变化，执行检查函数
    }

}

void Poke::on_action_xinyouxi_triggered()
{
    // 清理原有牌局
    this->clear();

    // 为发牌(动画)做初始准备
    auto *temp_stack_side = new QGraphicsPixmapItem(QPixmap(":/images/stack_side.png")); // 模拟发牌牌堆
    temp_stack_side->setPos(575, 198);
    scene->addItem(temp_stack_side);

    auto *temp_card_back = new QGraphicsPixmapItem(QPixmap(":/images/card_back.png")); // 模拟发牌牌堆
    temp_card_back->setPos(575, 0);
    scene->addItem(temp_card_back);

    QVector<Card*> cards; // 存贮40张卡牌，全部生成后随机打乱，最后开始发放

    Card *temp_flower = new Card(FLOWER, -1); // 生成“FLOWER”
    cards.push_back(temp_flower);

    for (int i = 0; i < 4; ++i) { // 生成另外三种特殊无点数花色(KUANG，FA，ZHONG)
        Card *temp_suit = nullptr;
        temp_suit = new Card(KUANG, -1);
        cards.push_back(temp_suit);
        temp_suit = new Card(FA, -1);
        cards.push_back(temp_suit);
        temp_suit = new Card(ZHONG, -1);
        cards.push_back(temp_suit);
    }

    for (int i = 1; i <= 9; ++i) { // 生成剩余三种有点数花色(BAMBOO，COINS，CHICHAR)
        Card *temp_rank = nullptr;
        temp_rank = new Card(BAMBOO, i);
        cards.push_back(temp_rank);
        temp_rank = new Card(COINS, i);
        cards.push_back(temp_rank);
        temp_rank = new Card(CHICHAR, i);
        cards.push_back(temp_rank);
    }

    std::shuffle(cards.begin(), cards.end(), *QRandomGenerator::global()); // 随机打乱牌组

    for (int i = 0, x = 7; i < 8; ++i, x += 152) { // 添加“哨兵”卡牌，方便当某列空置时添加卡牌
        Card *dummy_card = new Card(-10, -10, nullptr, cards[i]);

        cards[i]->set_pre(dummy_card);
        dummy_card->setFlag(QGraphicsItem::ItemIsSelectable, false);
        dummy_card->setFlag(QGraphicsItem::ItemIsMovable, false);
        dummy_card->setPos(x, 252);
        Card::tableau_push(i, dummy_card);
        scene->addItem(dummy_card);
    }

    for (int i = 0, x = 7, y = 252; i < 40; ++i) { // 开始发放40张卡牌
        cards[i]->setPos(575, i - i * 0.6); // 令卡牌从牌堆中发出
        scene->addItem(cards[i]);

        if (i % 8 == 0) { // 按序摆放牌堆
            x = 7;
            y += 26;
        } else {
            x += 152;
        }

        // 动画开始，不可移动
        cards[i]->setFlag(QGraphicsItem::ItemIsSelectable, false);
        cards[i]->setFlag(QGraphicsItem::ItemIsMovable, false);

        // 卡牌发牌动画与音效
        QPropertyAnimation *animation = new QPropertyAnimation(cards[i], "position");

        animation->setStartValue(QPointF(575, i - i * 0.6)); // 从牌堆发出
        animation->setEndValue(QPoint(x, y));
        animation->setDuration(100);
        animation->setEasingCurve(QEasingCurve::InOutCubic); // 先加速，后减速

        temp_card_back->setPos(575, i - i * 0.6); // 模拟牌堆减少的过程

        if (i == 39) { // 发放最后一张卡牌前需要特殊处理
            scene->removeItem(temp_card_back), scene->removeItem(temp_stack_side);
            delete temp_card_back, delete temp_stack_side;
        }

        QEventLoop loop;
        QObject::connect(animation, &QPropertyAnimation::finished, &loop, &QEventLoop::quit); // 阻塞当前代码直到动画完成
        animation->start();
        QSound::play(":/sounds/card_place.wav"); // 播放发牌音效
        loop.exec();
        delete animation;

        Card::tableau_push(i % 8, cards[i]);

        if (i > 7) {
            cards[i]->set_pre(cards[i - 8]); // 建立当前列的卡牌的前后联系(前后指针)
            cards[i - 8]->set_next(cards[i]);
        }

        QObject::connect(cards[i], &Card::card_change, this, &Poke::check); // 将每一个对象连接到检查函数
    }

    for (auto &card : cards) { // 动画结束，可移动
        card->setFlag(QGraphicsItem::ItemIsSelectable, true);
        card->setFlag(QGraphicsItem::ItemIsMovable, true);
    }

    this->check();
}

void Poke::on_action_help_triggered()
{
    QAction *help_action = ui->action_help;

    if (help_action->text() == "帮助(Help)") { // 展示游戏帮助
        help_action->setText("返回游戏(Back to game)");
        help_image->show();
    } else {
        help_action->setText("帮助(Help)");
        help_image->hide();
    }
}

void Poke::on_action_guanyu_triggered()
{
    QMessageBox::information(this, "关于", "SHENZHEN I/O Solitaire\nDeveloper: Zachtronics\nKrisCenGH: kris__cen@outlook.com");
}

void Poke::on_kuang_pressed()
{
    QPushButton *kuang_button = ui->kuang;

    kuang_button->setStyleSheet(
                "border-radius: 36px;"
                "color: transparent;"
                "background-image: url(:/images/button_white_down.png);"
                );
    kuang_button->setEnabled(false);

    // 收牌
    collect_cards(KUANG, nullptr, -1);
}

void Poke::on_fa_pressed()
{
    QPushButton *fa_button = ui->fa;

    fa_button->setStyleSheet(
                "border-radius: 36px;"
                "color: transparent;"
                "background-image: url(:/images/button_green_down.png);"
                );
    fa_button->setEnabled(false);

    // 收牌
    collect_cards(FA, nullptr, -1);
}

void Poke::on_zhong_pressed()
{
    QPushButton *zhong_button = ui->zhong;

    zhong_button->setStyleSheet(
                "border-radius: 36px;"
                "color: transparent;"
                "background-image: url(:/images/button_red_down.png);"
                );
    zhong_button->setEnabled(false);

    // 收牌
    collect_cards(ZHONG, nullptr, -1);
}
